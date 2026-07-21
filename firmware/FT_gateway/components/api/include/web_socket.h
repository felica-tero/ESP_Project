#ifndef __PERSONAL_WEB_SOCKET_H__
#define __PERSONAL_WEB_SOCKET_H__

void register_ws_route(void);
void broadcast_bomba_status(int bomba_id, const char *status);

#endif//__PERSONAL_WEB_SOCKET_H__