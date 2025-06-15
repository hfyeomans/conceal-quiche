#include "quiche/include/quiche.h"
#include "../include/masque.h"
#include <arpa/inet.h>
#include <netinet/in.h>

static uint64_t next_uni_stream_id = 2;  // Client-initiated unidirectional streams start at 2

uint64_t conceal_masque_stream_new(quiche_conn *conn)
{
    if (!conn) return UINT64_MAX;
    
    // Check if we can create a new stream
    if (quiche_conn_stream_capacity(conn, next_uni_stream_id) > 0) {
        uint64_t stream_id = next_uni_stream_id;
        next_uni_stream_id += 4;  // Client uni streams: 2, 6, 10, 14, ...
        return stream_id;
    }
    
    return UINT64_MAX;
}

quiche_conn *conceal_masque_connect(const uint8_t *scid,
                                    size_t scid_len,
                                    const char *server_name,
                                    uint16_t port)
{
    quiche_config *cfg = quiche_config_new(0xbabababa);
    quiche_config_set_application_protos(cfg,
        (uint8_t *)"\x05hq-29\x05hq-30\x05hq-31",  18);
    quiche_config_set_max_idle_timeout(cfg, 15000);
    quiche_config_enable_early_data(cfg);

    // Create socket addresses
    struct sockaddr_in local_addr = {0};
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = 0;  // Let OS choose port
    
    struct sockaddr_in peer_addr = {0};
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port = htons(port);
    peer_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // TODO: resolve server_name

    return quiche_connect(server_name,
                          (const uint8_t *)scid,
                          scid_len, 
                          (struct sockaddr *)&local_addr, sizeof(local_addr),
                          (struct sockaddr *)&peer_addr, sizeof(peer_addr),
                          cfg);
}

ssize_t conceal_masque_send(quiche_conn *conn,
                            uint64_t stream_id,
                            const uint8_t *buf,
                            size_t len)
{
    uint64_t error_code = 0;
    return quiche_conn_stream_send(conn, stream_id, buf, len, true, &error_code);
}

uint64_t conceal_masque_poll(quiche_conn *conn,
                             uint8_t *out_buf,
                             size_t buf_len)
{
    quiche_stream_iter *readable = quiche_conn_readable(conn);
    uint64_t sid = UINT64_MAX;
    if (quiche_stream_iter_next(readable, &sid)) {
        bool fin = false;
        uint64_t error_code = 0;
        quiche_conn_stream_recv(conn, sid, out_buf, buf_len, &fin, &error_code);
    }
    quiche_stream_iter_free(readable);
    return sid;
}
