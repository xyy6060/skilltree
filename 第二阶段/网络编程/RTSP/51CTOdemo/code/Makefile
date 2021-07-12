all:
	gcc rtsp-server-r/rtsp_server.c -lpthread -o rtsp-server-r/rtsp_server
	gcc rtsp-server-rw/rtsp_server.c rtsp-server-rw/rtsp.c -lpthread -o rtsp-server-rw/rtsp_server
	gcc rtp/rtp_send_av.c -lpthread -o rtp/rtp_send_av
	gcc rtsp-rtp-mix/rtp_send_av.c rtsp-rtp-mix/rtsp.c rtsp-rtp-mix/rtsp-rtp-mix.c -lpthread -o rtsp-rtp-mix/rtsp-rtp-mix

clean:
	rm rtsp-server-r/rtsp_server rtsp-server-rw/rtsp_server rtp/rtp_send_av rtsp-rtp-mix/rtsp-rtp-mix -rf
