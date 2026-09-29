// SPDX-License-Identifier: GPL-2.0-only
/*
 * Google Tensor Central Power Manager (CPM) interface
 *
 * The CPM is a microcontroller that owns most clocks, power domains and
 * resets of the SoC. The AP talks to it through two queued MBA mailboxes:
 * requests go out on one, responses and CPM-initiated notifications come
 * back on the other. Every message is four words, a header followed by
 * three payload words.
 *
 * This driver implements the message transport, the MBFS key/value service
 * used for resource votes, and spawns the clock and power-domain providers
 * as auxiliary devices.
 *
 * Based on the downstream Pixel drivers.
 *
 * Copyright 2023-2025 Google LLC
 */

#include <linux/auxiliary_bus.h>
#include <linux/bitfield.h>
#include <linux/cleanup.h>
#include <linux/completion.h>
#include <linux/mailbox/goog-mba-message.h>
#include <linux/mailbox_client.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/soc/google/google-cpm.h>
#include <linux/spinlock.h>
#include <linux/string.h>

#define CPM_MSG_WORDS		(1 + GOOGLE_CPM_PAYLOAD_WORDS)
#define CPM_TIMEOUT_MS		3000
#define CPM_MAX_NOTIFIERS	4

/* Message header */
#define HDR_DST			GENMASK(7, 0)
#define HDR_SRC			GENMASK(15, 8)
#define HDR_SEQ			GENMASK(19, 16)
#define HDR_TOKEN		GENMASK(23, 20)
#define HDR_TYPE		GENMASK(30, 29)
#define HDR_VALID		BIT(31)

#define HDR_TYPE_ONEWAY		0
#define HDR_TYPE_REQUEST	1
#define HDR_TYPE_RESPONSE	2

#define HDR_SEQ_MIN		1
#define HDR_SEQ_MAX		13
#define HDR_NUM_TOKENS		16

/* Sent once to (re)start the queued protocol, also sent by a restarted CPM. */
#define HDR_INIT_PROTO		0x100

/* MBFS service */
#define MBFS_CMD		GENMASK(31, 28)
#define MBFS_HANDLE		GENMASK(27, 0)
#define MBFS_ERR		GENMASK(31, 29)
#define MBFS_VER_MINOR		GENMASK(15, 0)
#define MBFS_VER_MAJOR		GENMASK(31, 16)

#define MBFS_GET_PROTOCOL_VERSION	0
#define MBFS_GET_ROOT_HANDLE		1
#define MBFS_FIND_CHILD_BY_BASE_NAME	4
#define MBFS_FIND_HOMONYM_BY_EXTENSION	5
#define MBFS_READ_FILE			7
#define MBFS_WRITE_FILE			8

#define MBFS_OK			0
#define MBFS_NOT_FOUND		2
#define MBFS_FORBIDDEN		3
#define MBFS_TRY_AGAIN		5

#define MBFS_NAME_LEN		16

struct google_cpm_notifier {
	u8 ap_service;
	google_cpm_notify_t fn;
	void *data;
};

struct google_cpm {
	struct device *dev;
	struct mbox_client tx_cl;
	struct mbox_client rx_cl;
	struct mbox_chan *tx;
	struct mbox_chan *rx;

	/* Serialises requests, only one is outstanding at a time. */
	struct mutex lock;
	u8 seq;
	u8 token;

	/* Protects the fields below, which the RX callback updates. */
	spinlock_t rx_lock;
	bool waiting;
	struct completion done;
	u32 resp[GOOGLE_CPM_PAYLOAD_WORDS];
	struct google_cpm_notifier notifiers[CPM_MAX_NOTIFIERS];

	u32 mbfs_root;
};

static int google_cpm_send(struct google_cpm *cpm, const u32 msg[CPM_MSG_WORDS],
			   bool init)
{
	struct goog_mba_tx_msg tx = {
		.payload = msg,
		.payload_words = CPM_MSG_WORDS,
		.init = init,
	};
	int ret;

	/* tx_block is set, so @msg stays in use only for the duration. */
	ret = mbox_send_message(cpm->tx, &tx);

	return ret < 0 ? ret : 0;
}

int google_cpm_request(struct google_cpm *cpm, u8 service,
		       const u32 req[GOOGLE_CPM_PAYLOAD_WORDS],
		       u32 resp[GOOGLE_CPM_PAYLOAD_WORDS])
{
	u32 msg[CPM_MSG_WORDS];
	int ret;

	guard(mutex)(&cpm->lock);

	cpm->token = (cpm->token + 1) % HDR_NUM_TOKENS;
	cpm->seq = cpm->seq >= HDR_SEQ_MAX ? HDR_SEQ_MIN : cpm->seq + 1;

	msg[0] = FIELD_PREP(HDR_DST, service) |
		 FIELD_PREP(HDR_SEQ, cpm->seq) |
		 FIELD_PREP(HDR_TOKEN, cpm->token) |
		 FIELD_PREP(HDR_TYPE, HDR_TYPE_REQUEST) |
		 HDR_VALID;
	memcpy(&msg[1], req, sizeof(u32) * GOOGLE_CPM_PAYLOAD_WORDS);

	reinit_completion(&cpm->done);
	scoped_guard(spinlock_irqsave, &cpm->rx_lock)
		cpm->waiting = true;

	ret = google_cpm_send(cpm, msg, false);
	if (!ret && !wait_for_completion_timeout(&cpm->done,
						 msecs_to_jiffies(CPM_TIMEOUT_MS)))
		ret = -ETIMEDOUT;

	scoped_guard(spinlock_irqsave, &cpm->rx_lock) {
		cpm->waiting = false;
		if (!ret)
			memcpy(resp, cpm->resp, sizeof(cpm->resp));
	}

	if (ret)
		dev_err(cpm->dev, "request to service %#x failed: %d\n",
			service, ret);

	return ret;
}
EXPORT_SYMBOL_GPL(google_cpm_request);

static void google_cpm_rx_callback(struct mbox_client *cl, void *data)
{
	struct google_cpm *cpm = container_of(cl, struct google_cpm, rx_cl);
	const struct goog_mba_rx_msg *rx = data;
	const u32 *msg = rx->payload;
	unsigned int i;
	u32 hdr;

	if (rx->payload_words < CPM_MSG_WORDS)
		return;

	hdr = msg[0];
	if (hdr == HDR_INIT_PROTO) {
		dev_warn(cpm->dev, "CPM restarted its mailbox protocol\n");
		return;
	}

	guard(spinlock_irqsave)(&cpm->rx_lock);

	switch (FIELD_GET(HDR_TYPE, hdr)) {
	case HDR_TYPE_RESPONSE:
		if (!cpm->waiting || FIELD_GET(HDR_TOKEN, hdr) != cpm->token) {
			dev_warn(cpm->dev, "unexpected response %#x\n", hdr);
			return;
		}
		memcpy(cpm->resp, &msg[1], sizeof(cpm->resp));
		cpm->waiting = false;
		complete(&cpm->done);
		return;
	case HDR_TYPE_REQUEST:
	case HDR_TYPE_ONEWAY:
		for (i = 0; i < CPM_MAX_NOTIFIERS; i++) {
			struct google_cpm_notifier *n = &cpm->notifiers[i];

			if (n->fn && n->ap_service == FIELD_GET(HDR_DST, hdr)) {
				n->fn(n->data, &msg[1]);
				return;
			}
		}
		dev_dbg(cpm->dev, "unhandled notification %#x\n", hdr);
		return;
	default:
		dev_warn(cpm->dev, "unknown message %#x\n", hdr);
	}
}

int google_cpm_register_notifier(struct google_cpm *cpm, u8 ap_service,
				 google_cpm_notify_t fn, void *data)
{
	unsigned int i;

	guard(spinlock_irqsave)(&cpm->rx_lock);

	for (i = 0; i < CPM_MAX_NOTIFIERS; i++) {
		struct google_cpm_notifier *n = &cpm->notifiers[i];

		if (!n->fn) {
			n->ap_service = ap_service;
			n->data = data;
			n->fn = fn;
			return 0;
		}
	}

	return -ENOSPC;
}
EXPORT_SYMBOL_GPL(google_cpm_register_notifier);

/* MBFS */

static int google_cpm_mbfs(struct google_cpm *cpm, u32 cmd, u32 handle,
			   u32 arg1, u32 arg2, u32 resp[GOOGLE_CPM_PAYLOAD_WORDS])
{
	u32 req[GOOGLE_CPM_PAYLOAD_WORDS] = {
		FIELD_PREP(MBFS_CMD, cmd) | FIELD_PREP(MBFS_HANDLE, handle),
		arg1,
		arg2,
	};
	int ret;

	ret = google_cpm_request(cpm, GOOGLE_CPM_SVC_MBFS, req, resp);
	if (ret)
		return ret;

	switch (FIELD_GET(MBFS_ERR, resp[0])) {
	case MBFS_OK:
		return 0;
	case MBFS_NOT_FOUND:
		return -ENOENT;
	case MBFS_FORBIDDEN:
		return -EACCES;
	case MBFS_TRY_AGAIN:
		return -EAGAIN;
	default:
		return -EIO;
	}
}

/*
 * Node names are up to 16 bytes, zero padded. The first 8 bytes are the
 * "base name" and the last 8 the "name extension"; siblings may share a base
 * name and are then told apart by their extension.
 */
int google_cpm_mbfs_lookup(struct google_cpm *cpm, u32 folder,
			   const char *name, u32 *handle)
{
	u32 padded[MBFS_NAME_LEN / sizeof(u32)] = { };
	u32 resp[GOOGLE_CPM_PAYLOAD_WORDS];
	int ret;

	if (strlen(name) > MBFS_NAME_LEN)
		return -ENAMETOOLONG;

	memcpy(padded, name, strlen(name));

	ret = google_cpm_mbfs(cpm, MBFS_FIND_CHILD_BY_BASE_NAME, folder,
			      padded[0], padded[1], resp);
	if (ret)
		return ret;

	if (resp[1] == padded[2] && resp[2] == padded[3]) {
		*handle = FIELD_GET(MBFS_HANDLE, resp[0]);
		return 0;
	}

	ret = google_cpm_mbfs(cpm, MBFS_FIND_HOMONYM_BY_EXTENSION,
			      FIELD_GET(MBFS_HANDLE, resp[0]),
			      padded[2], padded[3], resp);
	if (ret)
		return ret;

	*handle = FIELD_GET(MBFS_HANDLE, resp[0]);

	return 0;
}
EXPORT_SYMBOL_GPL(google_cpm_mbfs_lookup);

/* Look up a '/' separated path relative to the MBFS root folder. */
int google_cpm_mbfs_get_handle(struct google_cpm *cpm, const char *path,
			       u32 *handle)
{
	char name[MBFS_NAME_LEN + 1];
	u32 node = cpm->mbfs_root;
	const char *end;
	size_t len;
	int ret;

	while (*path) {
		end = strchrnul(path, '/');
		len = end - path;
		if (len) {
			if (len > MBFS_NAME_LEN)
				return -ENAMETOOLONG;

			memcpy(name, path, len);
			name[len] = '\0';

			ret = google_cpm_mbfs_lookup(cpm, node, name, &node);
			if (ret)
				return ret;
		}
		path = *end ? end + 1 : end;
	}

	*handle = node;

	return 0;
}
EXPORT_SYMBOL_GPL(google_cpm_mbfs_get_handle);

int google_cpm_mbfs_read(struct google_cpm *cpm, u32 handle, u64 *val)
{
	u32 resp[GOOGLE_CPM_PAYLOAD_WORDS];
	int ret;

	ret = google_cpm_mbfs(cpm, MBFS_READ_FILE, handle, 0, 0, resp);
	if (ret)
		return ret;

	*val = (u64)resp[2] << 32 | resp[1];

	return 0;
}
EXPORT_SYMBOL_GPL(google_cpm_mbfs_read);

/* Returns -EAGAIN if the CPM accepted the write but completes it later. */
int google_cpm_mbfs_write(struct google_cpm *cpm, u32 handle, u64 val)
{
	u32 resp[GOOGLE_CPM_PAYLOAD_WORDS];

	return google_cpm_mbfs(cpm, MBFS_WRITE_FILE, handle, lower_32_bits(val),
			       upper_32_bits(val), resp);
}
EXPORT_SYMBOL_GPL(google_cpm_mbfs_write);

static int google_cpm_mbfs_init(struct google_cpm *cpm)
{
	u32 resp[GOOGLE_CPM_PAYLOAD_WORDS];
	int ret;

	ret = google_cpm_mbfs(cpm, MBFS_GET_PROTOCOL_VERSION, 0, 0, 0, resp);
	if (ret)
		return ret;

	if (FIELD_GET(MBFS_VER_MAJOR, resp[1]) != 0 ||
	    FIELD_GET(MBFS_VER_MINOR, resp[1]) < 1)
		return dev_err_probe(cpm->dev, -EPROTONOSUPPORT,
				     "unsupported MBFS version %#x\n", resp[1]);

	ret = google_cpm_mbfs(cpm, MBFS_GET_ROOT_HANDLE, 0, 0, 0, resp);
	if (ret)
		return ret;

	cpm->mbfs_root = FIELD_GET(MBFS_HANDLE, resp[0]);

	return 0;
}

struct google_cpm *google_cpm_get(struct device *dev)
{
	return dev_get_drvdata(dev->parent);
}
EXPORT_SYMBOL_GPL(google_cpm_get);

static void google_cpm_free_channel(void *chan)
{
	mbox_free_channel(chan);
}

static int google_cpm_probe(struct platform_device *pdev)
{
	static const u32 init_msg[CPM_MSG_WORDS] = { HDR_INIT_PROTO };
	struct device *dev = &pdev->dev;
	struct auxiliary_device *adev;
	struct google_cpm *cpm;
	int ret;

	cpm = devm_kzalloc(dev, sizeof(*cpm), GFP_KERNEL);
	if (!cpm)
		return -ENOMEM;

	cpm->dev = dev;
	mutex_init(&cpm->lock);
	spin_lock_init(&cpm->rx_lock);
	init_completion(&cpm->done);
	platform_set_drvdata(pdev, cpm);

	cpm->tx_cl.dev = dev;
	cpm->tx_cl.tx_block = true;
	cpm->tx_cl.tx_tout = CPM_TIMEOUT_MS;
	cpm->tx = mbox_request_channel_byname(&cpm->tx_cl, "tx");
	if (IS_ERR(cpm->tx))
		return dev_err_probe(dev, PTR_ERR(cpm->tx), "no tx mailbox\n");
	ret = devm_add_action_or_reset(dev, google_cpm_free_channel, cpm->tx);
	if (ret)
		return ret;

	cpm->rx_cl.dev = dev;
	cpm->rx_cl.rx_callback = google_cpm_rx_callback;
	cpm->rx = mbox_request_channel_byname(&cpm->rx_cl, "rx");
	if (IS_ERR(cpm->rx))
		return dev_err_probe(dev, PTR_ERR(cpm->rx), "no rx mailbox\n");
	ret = devm_add_action_or_reset(dev, google_cpm_free_channel, cpm->rx);
	if (ret)
		return ret;

	ret = google_cpm_send(cpm, init_msg, true);
	if (ret)
		return dev_err_probe(dev, ret, "failed to start the CPM protocol\n");

	ret = google_cpm_mbfs_init(cpm);
	if (ret)
		return dev_err_probe(dev, ret, "failed to initialise MBFS\n");

	adev = devm_auxiliary_device_create(dev, "clk", NULL);
	if (!adev)
		return dev_err_probe(dev, -ENODEV, "failed to create clock device\n");

	adev = devm_auxiliary_device_create(dev, "pd", NULL);
	if (!adev)
		return dev_err_probe(dev, -ENODEV,
				     "failed to create power domain device\n");

	return 0;
}

static const struct of_device_id google_cpm_of_match[] = {
	{ .compatible = "google,mbu-cpm" },
	{ }
};
MODULE_DEVICE_TABLE(of, google_cpm_of_match);

static struct platform_driver google_cpm_driver = {
	.probe = google_cpm_probe,
	.driver = {
		.name = "google-cpm",
		.of_match_table = google_cpm_of_match,
		.suppress_bind_attrs = true,
	},
};
module_platform_driver(google_cpm_driver);

MODULE_DESCRIPTION("Google Tensor CPM interface");
MODULE_LICENSE("GPL");
