/*
 * This file is part of the zlog Library.
 *
 * ntp client
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>

#include "ntp_client.h"

in_addr_t inet_host(const char *host)
{
    in_addr_t saddr;
    struct hostent *hostent;

    if ((saddr = inet_addr(host)) == INADDR_NONE) {
        if ((hostent = gethostbyname(host)) == NULL)
            return INADDR_NONE;

        memmove(&saddr, hostent->h_addr, hostent->h_length);
    }
	
    return saddr;
}

/*
 *  ��ȡ�ͻ������������ʱ��ƫ����
 *      ����ntpָ�������Ӧ�����ڵĻ�������
 *      ����recvtvָ���յ�������Ӧ��ı���ʱ�䣻
 */
static double get_offset(const struct ntphdr *ntp, const struct timeval *recvtv)
{
    double t1, t2, t3, t4;

    t1 = NTP_LFIXED2DOUBLE(&ntp->ntp_orits);
    t2 = NTP_LFIXED2DOUBLE(&ntp->ntp_recvts);
    t3 = NTP_LFIXED2DOUBLE(&ntp->ntp_transts);
    t4 = recvtv->tv_sec + recvtv->tv_usec / 1000000.0;

    return ((t2 - t1) + (t3 - t4)) / 2;
}

/*
 *  ����һ��NTP������
 *      c����bufָ����NTP���ĵĻ�������
 *      sizeΪֵ-���������������Ϊ���������ȣ�����ʱΪNTP���ĳ���
 */
int get_ntp_packet(void *buf, size_t *size)
{
	struct ntphdr *ntp;
	struct timeval tv;
	
	if(buf == NULL || size == NULL || *size < NTP_HLEN)
		return -1;

	memset(buf, 0, *size);
	
	ntp = (struct ntphdr*)buf;
	ntp->ntp_li = NTP_LI;
	ntp->ntp_vn = NTP_VN;
	ntp->ntp_mode = NTP_MODE;
	ntp->ntp_stratum = NTP_STRATUM;
	ntp->ntp_poll = NTP_POLL;
	ntp->ntp_precision = NTP_PRECISION;

	gettimeofday(&tv, NULL);
	ntp->ntp_transts.intpart = htonl(tv.tv_sec + JAN_1970);
	ntp->ntp_transts.fracpart = htonl(USEC2FRAC(tv.tv_usec));

	*size = NTP_HLEN;
	
	return 0;
}

int ntp_sync_time(int time_out)
{
	int sockfd, maxfd;
	fd_set readfds;
	struct sockaddr_in  servaddr;
	char buf[NTP_BUF_SIZE];
	size_t nbytes;
	struct timeval timeout, recvtv, tv;
	double offset = 0;

	//������������ַ�׽��ֽṹ
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("[%s][%d]socket error\n", __func__, __LINE__);
		return -1;
	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(NTP_PORT);
	servaddr.sin_addr.s_addr = inet_host(NTP_ADDR);
	if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(struct sockaddr)) != 0)
	{
		printf("[%s][%d]connect error\n", __func__, __LINE__);
		close(sockfd);
		return -1;
	}

	//����������NTP������
	nbytes = sizeof(buf);
	if(get_ntp_packet(buf, &nbytes) != 0)
	{
		printf("[%s][%d]construct ntp request error\n", __func__, __LINE__);
		close(sockfd);
		return -1;
	}
	send(sockfd, buf, nbytes, 0);

	//����select��ʱʱ��
	timeout.tv_sec = time_out;
	timeout.tv_usec = 0;
	FD_ZERO(&readfds);
	FD_SET(sockfd, &readfds);
	maxfd = sockfd +1;
	if(select(maxfd, &readfds, NULL, NULL, &timeout) > 0)
	{
		if(FD_ISSET(sockfd, &readfds))
		{
			if((nbytes = recv(sockfd, buf, sizeof(buf), 0)) < 0)
			{
				printf("[%s][%d]recv error\n", __func__, __LINE__);
				close(sockfd);
				return -1;
			}
			//����ͻ���ʱ�����������ʱ��ƫ����
			gettimeofday(&recvtv, NULL);
			offset = get_offset((struct ntphdr *) buf, &recvtv);

			//����ϵͳʱ��
			gettimeofday(&tv, NULL);
			printf("[%s][%d] now: %s, start set time ,time offset = %.10f \n", __func__, __LINE__, ctime((time_t *) &tv.tv_sec),  offset);
			tv.tv_sec += (int) offset;
			tv.tv_usec += offset - (int) offset;
			if (settimeofday(&tv, NULL) != 0)
			{
				printf("[%s][%d]settimeofday error\n", __func__, __LINE__);
				close(sockfd);
				return -1;
			}
			printf("[%s][%d]set time success,now:%s \n", __func__, __LINE__, ctime((time_t *) &tv.tv_sec));
		}
	}
	else
	{
		printf("[%s][%d]timeout or error\n", __func__, __LINE__);
		close(sockfd);
		return -1;
	}

	//������ɺ�����˹ر��׽���
	close(sockfd);

	return 0;
}
