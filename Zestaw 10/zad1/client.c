#include "addictionally.h"


int fd;

void handle_sigint(int tmp)
{
    printf("STOP\n");
    MESSAGE mess;
    mess.type = STOP;
    tmp = write(fd, &mess, sizeof(mess));
    shutdown(fd, SHUT_RDWR);
    close(fd);
    exit(0);
}


void handle_stdin_input()
{
    char buffer[MESSAGE_LENGTH];
    int size = read(STDIN_FILENO, &buffer, MESSAGE_LENGTH);
    if (size == 0)
    {
        close(fd);
        exit(0);
    }
    buffer[size] = 0;


    char separator[] = " \t\n";
    char *command;
    command = strtok(buffer, separator);

    MESSAGETYPE message_type = -1;
    char another[MESSAGE_LENGTH];
    char text[MESSAGE_LENGTH];
    if (command == NULL)
        return;


    if (strcmp(command, "LIST") == 0)
    {
        message_type = LIST;
    }
    else if (strcmp(command, "2ALL") == 0)
    {
        message_type = TOALL;


        command = strtok(NULL, "\n");
        if (command != NULL)
        {
            memcpy(text, command, strlen(command) * sizeof(char));
            text[strlen(command)] = 0;
        }
    }
    else if (strcmp(command, "2ONE") == 0)
    {
        message_type = TOONE;


        command = strtok(NULL, separator);
        if (command != NULL)
        {
            memcpy(another, command, strlen(command) * sizeof(char));
            another[strlen(command)] = 0;
        }
        else
        {
            printf("Nieprawidlowe uzycie komendy 2ONE\n");
            return;
        }


        command = strtok(NULL, "\n");
        if (command != NULL)
        {
            memcpy(text, command, strlen(command) * sizeof(char));
            text[strlen(command)] = 0;
        }
        else
        {
            printf("Nieprawidlowe uzycie komendy 2ONE\n");
            return;
        }
    }
    else if (strcmp(command, "STOP") == 0)
    {
        message_type = STOP;
    }
    else
    {
        printf("Nieznana komenda\n");
        message_type = -1;
    }


    MESSAGE mess;
    mess.type = message_type;
    memcpy(&mess.another, another, strlen(another) + 1);
    memcpy(&mess.content, text, strlen(text) + 1);


    write(fd, &mess, sizeof(mess));
    fflush(stdout);
}


void handle_socket_input()
{
    MESSAGE mess;
    read(fd, &mess, sizeof(mess));


    switch (mess.type)
    {
    case NICK:
        printf("Zajeta nazwa\n");
        close(fd);
        exit(0);
    case FULL:
        printf("Wszytskie miejsca zajete\n");
        close(fd);
        exit(0);
    case PING:
        write(fd, &mess, sizeof(mess));
        break;
    case STOP:
        printf("STOP\n");
        close(fd);
        exit(0);
    case GET:
        printf("%s\n", mess.content);
        fflush(stdout);
        break;
    default:
        close(fd);
        printf("Nieznany typ wiadomosci\n");
        exit(0);
    }
}


void setup_epoll(int fd_epoll)
{
    struct epoll_event stdin_event;
    stdin_event.events = EPOLLIN | EPOLLPRI;
    stdin_event.data.fd = STDIN_FILENO;
    epoll_ctl(fd_epoll, EPOLL_CTL_ADD, STDIN_FILENO, &stdin_event);


    struct epoll_event socket_event;
    socket_event.events = EPOLLIN | EPOLLPRI;
    socket_event.data.fd = fd;
    epoll_ctl(fd_epoll, EPOLL_CTL_ADD, fd, &socket_event);
}


void start_chat(int fd_epoll)
{
    struct epoll_event events[2];
    while (1)
    {
        int nread = epoll_wait(fd_epoll, events, 2, 10);
        for (int i = 0; i < nread; i++)
        {
            if (events[i].data.fd == STDIN_FILENO)
            {
                handle_stdin_input();
            }
            else
            {
                handle_socket_input();
            }
        }
    }
}
int connecting_unix(char *unixpath)
{
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, unixpath, sizeof(addr.sun_path) - 1);
    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    connect(sfd, (struct sockaddr *)&addr, sizeof(addr));
    return sfd;
}


int connecting_inet(char *ip, int inet_port)
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(inet_port);
    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0)
    {
        perror("inet_pton error");
        exit(0);
    }
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(sfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect error");
        exit(1);
    }


    return sfd;
}


int main(int argc, char **argv)
{
    if (strcmp(argv[2], "inet") == 0 && argc == 5)
        fd = connecting_inet(argv[3], atoi(argv[4]));
    else if (strcmp(argv[2], "unix") == 0 && argc == 4)
        fd = connecting_unix(argv[3]);
    else
    {
        printf("Zla liczba argumentow\n");
        exit(0);
    }


    signal(SIGINT, handle_sigint);
    char *clientname = argv[1];
    write(fd, clientname, strlen(clientname));


    int fd_epoll = epoll_create1(0);
    setup_epoll(fd_epoll);
    start_chat(fd_epoll);
}





