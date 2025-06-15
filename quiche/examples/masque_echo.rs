use quiche::*;
use quiche::h3::*;
use std::{env, net::UdpSocket, time::Instant};

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args: Vec<_> = env::args().collect();
    let bind = &args[1];
    let socket = UdpSocket::bind(bind)?;
    println!("MASQUE echo listening on {}", bind);

    let mut buf  = [0; 65535];
    let mut scid = [0; 16];
    ring::rand::SystemRandom::new().fill(&mut scid).unwrap();

    let mut conn = quiche::accept(&scid, None, bind, bind, &mut default_config())?;
    let mut h3_cfg = quiche::h3::Config::new()?;
    let mut h3_conn = quiche::h3::Connection::with_transport(&mut conn, &h3_cfg)?;

    loop {
        let (len, peer) = socket.recv_from(&mut buf)?;
        let read = conn.recv(&mut buf[..len])?;

        for _ in 0..read.dgrams() {
            while let Ok((stream_id, _)) = h3_conn.poll(&mut conn) {
                while let Ok((hdrs, data)) =
                    h3_conn.recv_body(&mut conn, stream_id, &mut vec![0; 4096])
                {
                    h3_conn.send_response(&mut conn, stream_id, &hdrs, false)?;
                    h3_conn.send_body(&mut conn, stream_id, &data, true)?;
                }
            }
        }

        while let Ok((write, addr)) = conn.send(&mut buf) {
            socket.send_to(&buf[..write], addr)?;
        }
        if conn.is_closed() { break }
    }
}

fn default_config() -> quiche::Config {
    let mut cfg = quiche::Config::new(0xbaba_cafe).unwrap();
    cfg.set_application_protos(b"\x05hq-29\x05hq-30\x05hq-31").unwrap();
    cfg
}
