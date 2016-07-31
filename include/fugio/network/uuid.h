#ifndef NETWORK_UUID_H
#define NETWORK_UUID_H

#include <QUuid>

#define NID_NETWORK_REQUEST			(QUuid("{4231a257-5fa6-4ef4-8e78-5b3611da415a}"))

#define NID_TCP_SEND				(QUuid("{84163E8D-C110-48E1-89F3-7627B6BA3BC8}"))
#define NID_TCP_RECEIVE				(QUuid("{F2549472-D7C3-4A83-91D2-0A82BD896F61}"))
#define NID_TCP_SEND_RAW			(QUuid("{846EB0DA-C72F-4CB6-9F1C-A8D67AD95F81}"))
#define NID_TCP_RECEIVE_RAW			(QUuid("{B98E5492-E01A-4302-922D-112F7253AB54}"))

#define NID_UDP_RECEIVE_RAW			(QUuid("{6E3A0358-2325-4FDB-98E3-DF542F0691D3}"))
#define NID_UDP_SEND_RAW			(QUuid("{9B65DF8B-0AD7-4608-BCA9-94194828822D}"))

#define NID_SLIP_DECODE				(QUuid("{F1B4D012-421B-4354-B92F-A323A33A4975}"))
#define NID_SLIP_ENCODE				(QUuid("{281677c4-72eb-44d1-89c1-17c8e9090b35}"))

#endif // NETWORK_UUID_H
