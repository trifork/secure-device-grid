#ifndef MDGEXT_FILEDOWNLOAD
#define MDGEXT_FILEDOWNLOAD

typedef enum {
  mdgext_filedownload_completed_success = 0,
  mdgext_filedownload_size_known = 1,
  mdgext_filedownload_datablock_included = 2,  /* Data+count is the current data package */
  mdgext_filedownload_checksum_valid = 3,      /* Data+count is the checksum */
  mdgext_filedownload_no_update_available = 4, /* Update server contacted, said nothing new for me. */
  mdgext_filedownload_failed = 32,             /* cancelled or failed in unspecified way. */
  mdgext_filedownload_checksum_failed = 33,    /* Or length, signature, other internal validation. */
  mdgext_filedownload_connection_failed = 34,  /* Unable to lookup/connect to endpoint. */
  mdgext_filedownload_starting_check = 36,     /* Starting check now. */
} mdgext_filedownload_status;

/* Checksum is sha512 of the file contents in the order they arrive.
 * Calculated incrementally using
 * crypto_hash_sha512_init, crypto_hash_sha512_update, crypto_hash_sha512_final
 * as provided by mdg_sha512.c (and libsodium using the same api)
 */

/** Arguments sent when invoking mdgext_filedownload_cb:
 * `data` = memory buffer holding data received. Only valid during invocation of call.
 * `count` = number of bytes in buffer
 * `status` = current state of downloading as defined by enum mdgext_filedownload_status
 *
 * data is valid only when (status==mdgext_filedownload_datablock_included || status==mdgext_filedownload_checksum_valid)
 * count is valid only for (status==mdgext_filedownload_datablock_included || status==mdgext_filedownload_size_known  || status==mdgext_filedownload_checksum_valid)
 *
 * Data arrives in the order they are sent from the server, one block at a time. Blocksize is not fixed.
*/
typedef uint32_t (*mdgext_filedownload_cb)(const uint8_t *data, uint32_t count,
                                           mdgext_filedownload_status status);

/** init must be called once only before any other use of this module.
 *
 * Returns 0 on success.
 */
_MDG_API_ uint32_t mdgext_filedownload_init();

/** Initiate a file download. If a download is already active, cancel that too.
 * `cb` = Callback to invoke when data arrives and on status changes.
 * `hostname`, `portno` = endpoint of server software as dnsname or IP.
 * `properties` = key/value set of identifiers to ship to server upon connection.
 * `signing_public` = pointer to trust for file: Checksum match when decrypted from this sender.
 * `retry_seconds` = delay before retrying. Set this to e.g (24*3600) to look for update once every day.
 * When retry_seconds is non-zero, the download is retried for ever until cancel is invoked.
 *
 * Returns 0 for success.
*/
_MDG_API_ uint32_t mdgext_filedownload_start(mdgext_filedownload_cb cb,
                                             const mdg_configuration *ota_servers,
                                             mdg_property_t *properties,
                                             uint8_t *signing_public,
                                             uint32_t retry_seconds);

/** Cancel a file download. Invokes cb with status=mdgext_filedownload_failed.
 * Returns 0 on success.
*/
_MDG_API_ uint32_t mdgext_filedownload_cancel(mdgext_filedownload_cb cb);

/** Validate checksum of a block of memory against a known checksum.
 *  Use this to later verify that a downloaded file still matches the checksum
 *  it had when we downloaded it. It uses the same algorithm as the download.
 *
 * Returns 0 on success, nonzero on checksum mismatch.
*/
_MDG_API_ uint32_t mdgext_filedownload_verify_checksum(const uint8_t *data,
                                                       const uint32_t count,
                                                       const uint8_t *checksum);

#endif /*MDGEXT_FILEDOWNLOAD */
