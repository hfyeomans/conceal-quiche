// Stub implementation for quiche functions
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "quiche/include/quiche.h"

// Stub implementations
quiche_config *quiche_config_new(uint32_t version) {
    return (quiche_config *)0x1; // Return a non-null pointer
}

int quiche_config_set_application_protos(quiche_config *config,
                                         const uint8_t *protos,
                                         size_t protos_len) {
    return 0; // Success
}

void quiche_config_set_max_idle_timeout(quiche_config *config, uint64_t v) {
    // No-op
}

void quiche_config_enable_early_data(quiche_config *config) {
    // No-op
}

quiche_conn *quiche_connect(const char *server_name,
                           const uint8_t *scid, size_t scid_len,
                           const struct sockaddr *local, socklen_t local_len,
                           const struct sockaddr *peer, socklen_t peer_len,
                           quiche_config *config) {
    return (quiche_conn *)0x1; // Return a non-null pointer
}

ssize_t quiche_conn_stream_capacity(const quiche_conn *conn, uint64_t stream_id) {
    return 1; // Always has capacity
}

ssize_t quiche_conn_stream_send(quiche_conn *conn, uint64_t stream_id,
                               const uint8_t *buf, size_t buf_len, bool fin,
                               uint64_t *error_code) {
    return buf_len; // Pretend we sent everything
}

quiche_stream_iter *quiche_conn_readable(const quiche_conn *conn) {
    return (quiche_stream_iter *)0x1; // Return a non-null pointer
}

bool quiche_stream_iter_next(quiche_stream_iter *iter, uint64_t *stream_id) {
    return false; // No readable streams
}

void quiche_stream_iter_free(quiche_stream_iter *iter) {
    // No-op
}

ssize_t quiche_conn_stream_recv(quiche_conn *conn, uint64_t stream_id,
                               uint8_t *out, size_t buf_len, bool *fin,
                               uint64_t *error_code) {
    return 0; // No data received
}

// Additional stub implementations for test support
ssize_t quiche_conn_recv(quiche_conn *conn, uint8_t *buf, size_t buf_len,
                         const quiche_recv_info *info) {
    return buf_len; // Pretend we processed the data
}

ssize_t quiche_conn_send(quiche_conn *conn, uint8_t *out, size_t out_len,
                         quiche_send_info *out_info) {
    return 0; // No data to send
}