#include "weirserv.h"
/*******************NETLINK SOCKET*******************************/
//For the kernel netlink socket
struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
int sock_fd;
struct msghdr msg;
//Send msg to kernel
int send_message(char *msgtext)
{
	// Using netlink to talk to the kernel
        memset(&dest_addr, 0, sizeof(dest_addr));
        dest_addr.nl_family = AF_NETLINK;
        dest_addr.nl_pid = 0;   // For Linux Kernel 
        dest_addr.nl_groups = 0;  //unicast 
        dest_addr.nl_pad=0;
        nlh=(struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));

        // Fill the netlink message header 
        nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
        memset(nlh,0,MAX_PAYLOAD);
        nlh->nlmsg_pid = getpid();  // self pid 
        nlh->nlmsg_flags |= NLM_F_ACK;
        nlh->nlmsg_type = 0;
        nlh->nlmsg_len=MAX_PAYLOAD;
        // Fill in the netlink message payload 
        strcpy((char*)NLMSG_DATA(nlh), msgtext);
	//__android_log_print(ANDROID_LOG_DEBUG, TAG, "Sending message to kernel: %s\n", (char*)(NLMSG_DATA(nlh)));
        iov.iov_base = (void *)nlh;
        iov.iov_len = nlh->nlmsg_len;

        //Send the message
        memset(&msg,0,sizeof(msg));
        msg.msg_name = (void *)&dest_addr;
        msg.msg_namelen = sizeof(dest_addr);
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        int error=sendmsg(sock_fd, &msg, 0);
	free(nlh);
	//__android_log_print(ANDROID_LOG_DEBUG, TAG, "Message Send Error: %d\n", error);
	return error;
}

//Receive msg from kernel (also sends a reply)
int receive_message()
{	
	memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
	int retval=recvmsg(sock_fd, &msg, 0);
	//__android_log_print(ANDROID_LOG_DEBUG, TAG, "Message Receive Error: %d\n", retval);
	//__android_log_print(ANDROID_LOG_DEBUG, TAG, "Original message from kernel: %s\n", (char*)(NLMSG_DATA(nlh)));
	char* rmsgtext = (char*)malloc(nlh->nlmsg_len);
        strncpy(rmsgtext, NLMSG_DATA(nlh), nlh->nlmsg_len);
	//__android_log_print(ANDROID_LOG_DEBUG, TAG, "Copied Original message from kernel: %s\n", rmsgtext);

	long int calling_thread_id;
	char* data_from_kernel = (char*)malloc(nlh->nlmsg_len);
	sscanf(rmsgtext, "%ld;%s", &calling_thread_id, data_from_kernel);
	//__android_log_print(ANDROID_LOG_DEBUG, TAG, "Split message from kernel: for process %ld|%s\n", calling_thread_id, data_from_kernel);
	
	int result = weirQuery(data_from_kernel);
	char terminator='\0';
	char * reply_text =(char*) malloc(sizeof(long)+sizeof(int)+1);
	sprintf(reply_text, "%ld;%d%c", calling_thread_id, result, terminator);

	//__android_log_print(ANDROID_LOG_DEBUG, TAG, "Replying with message: for process %s\n", reply_text);
	send_message(reply_text);
	free(rmsgtext);
	free(data_from_kernel);
	free(reply_text);
	return retval;
}

int main(int argc, char **argv)
{
    	int res=0;
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "weirserv daemon started.\n");
    
    	//Sending message to the kernel
    	char* msgtext = "hello from weirserv daemon\0";

	//Sending a netlink hello to the kernel	
	//Creating the socket
        sock_fd = socket(AF_NETLINK, SOCK_RAW,NETLINK_HELLO);
        memset(&src_addr, 0, sizeof(src_addr));
        src_addr.nl_family = AF_NETLINK;
        src_addr.nl_pid = getpid();  // self pid 
        src_addr.nl_groups = 0;  //not in mcast groups 
        src_addr.nl_pad=0;
        bind(sock_fd, (struct sockaddr*)&src_addr,sizeof(src_addr));

	//Sending the initial message
    	res = send_message(msgtext);
    	if(res<0)
		goto out;

    	//Waiting for the kernel's message
   	while(1){
		res = receive_message();
		if(res<0){
			goto out;
		}
		
	}
    	//return result
   	out: 
		if(sock_fd) close(sock_fd);
		return res;
}
