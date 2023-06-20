#include "addictionally.h"


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


typedef struct event_data
{
    EVENT type;
    data data;
} event_data;


void send_msg(client *client, MESSAGETYPE type, char text[MESSAGE_LENGTH]);
void handle_INIT_state(client *client);
void handle_DEFAULT_state(client *client);
void message_to_client(client *client);
void delete_client(client *client);
client *handle_client(int client_fd);
void make_socket(int socket, void *addr, int addr_size);
void *pinging();
void handle_sigint();
void initialize_server(int port, char *socket_path);
void run_server();
void handle_socket_event(event_data *data);
void handle_client_event(event_data *data, struct epoll_event *event);


int descriptor_epoll;
client server_clients_arr[MAX_NUMBER_OF_CLIENTS];


void send_msg(client *client, MESSAGETYPE type, char text[MESSAGE_LENGTH])
{
    MESSAGE mess;
    mess.type = type;
    strncpy(mess.content, text, MESSAGE_LENGTH);
    write(client->fd, &mess, sizeof(mess));
}
void send_end(client *client, MESSAGETYPE type, char text[1])
{
    MESSAGE mess;
    mess.type = type;
    strncpy(mess.content, text, MESSAGE_LENGTH);
    write(client->fd, &mess, sizeof(mess));
}

void handle_INIT_state(client *client)
{
    int nick_size = read(client->fd, client->nick, sizeof(client->nick) - 1);
    pthread_mutex_lock(&mutex);
    int i = 0;
    while (i < MAX_NUMBER_OF_CLIENTS && (&server_clients_arr[i] == client || strncmp(client->nick, server_clients_arr[i].nick, nick_size) != 0))
    {
        i++;
    }


    if (i == MAX_NUMBER_OF_CLIENTS)
    {
        client->nick[nick_size] = '\0';
        client->state = READY;
        printf("Nowy klient %s\n", client->nick);
    }
    else
    {
        MESSAGE mess;
        mess.type = NICK;
        printf("Nazwa %s jest już zajęta. Klient nie został zarejestrowany.\n", client->nick);
        write(client->fd, &mess, sizeof(mess));
        strcpy(client->nick, "nie zarejestrowany");
        delete_client(client);
    }
    pthread_mutex_unlock(&mutex);
}


void handle_DEFAULT_state(client *client)
{
    MESSAGE mess;
    read(client->fd, &mess, sizeof(mess));
    printf("KLIENT %s ", client->nick);
    switch (mess.type)
    {
    case PING:
    {
        pthread_mutex_lock(&mutex);
        printf("odpowiada\n");
        client->respond = true;
        pthread_mutex_unlock(&mutex);
        break;
    }
    case LIST:
    {
        printf("Nasłuchiwanie\n");
        int i = 0;
        while (i < MAX_NUMBER_OF_CLIENTS)
        {
            if (server_clients_arr[i].state != EMPTY)
            {
                send_msg(client, GET, server_clients_arr[i].nick);
            }
            i++;
        }
        break;
    }
    case TOONE:
    {
        printf("Wysyłanie wiadomości do %s\n", mess.another);
        char MESSAGE[MESSAGE_LENGTH] = "";
        strcat(MESSAGE, client->nick);
        strcat(MESSAGE, ": ");
        strcat(MESSAGE, mess.content);


        int i = 0;
        while (i < MAX_NUMBER_OF_CLIENTS)
        {
            if (server_clients_arr[i].state != EMPTY && strcmp(server_clients_arr[i].nick, mess.another) == 0)
            {
                send_msg(server_clients_arr + i, GET, MESSAGE);
            }
            i++;
        }
        break;
    }
    case TOALL:
    {
        printf("Wysyłanie wiadomości do wszystkich\n");
        char MESSAGE[MESSAGE_LENGTH] = "";
        strcat(MESSAGE, client->nick);
        strcat(MESSAGE, ": ");
        strcat(MESSAGE, mess.content);
        int i = 0;
        while (i < MAX_NUMBER_OF_CLIENTS)
        {
            if (server_clients_arr[i].state != EMPTY)
            {
                send_msg(server_clients_arr + i, GET, MESSAGE);
            }
            i++;
        }
        break;
    }
    case STOP:
    {
        printf("STOP\n");
        pthread_mutex_lock(&mutex);
        delete_client(client);
        pthread_mutex_unlock(&mutex);
        break;
    }
    default:
        break;
    }
}


void message_to_client(client *client)
{
    switch (client->state)
    {
    case INIT:
        handle_INIT_state(client);
        break;
    default:
        handle_DEFAULT_state(client);
        break;
    }
}
void delete_client(client *client)
{
    printf("Usuwanie klienta %s\n", client->nick);
    client->state = EMPTY;
    client->nick[0] = '\0';
    epoll_ctl(descriptor_epoll, EPOLL_CTL_DEL, client->fd, NULL);
    close(client->fd);
    usleep(10000);
    memset(client, 0, sizeof(*client));
    client->fd = -1;
    client->state = EMPTY;
}
client *handle_client(int client_fd)
{
    pthread_mutex_lock(&mutex);
    int i;
    for (i = 0; i < MAX_NUMBER_OF_CLIENTS; i++)
    {
        if (server_clients_arr[i].state == EMPTY)
        {
            break;
        }
    }
    if (i == MAX_NUMBER_OF_CLIENTS)
    {
        pthread_mutex_unlock(&mutex);
        return NULL;
    }
    client *client = &server_clients_arr[i];


    client->fd = client_fd;
    client->state = INIT;
    client->respond = true;
    pthread_mutex_unlock(&mutex);
    return client;
}


void make_socket(int socket, void *addr, int addr_size)
{
    int reuse = 1;
    setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    bind(socket, (struct sockaddr *)addr, addr_size);
    listen(socket, MAX_NUMBER_OF_CLIENTS);
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLPRI;
    event_data *event_data_ptr = malloc(sizeof(event_data));
    event_data_ptr->type = SOCKET;
    event_data_ptr->data.socket = socket;
    event.data.ptr = event_data_ptr;
    epoll_ctl(descriptor_epoll, EPOLL_CTL_ADD, socket, &event);
}


void *pinging()
{
    static MESSAGE mess;
    mess.type = PING;
    while (true)
    {
        sleep(PING_TIMER);
        pthread_mutex_lock(&mutex);
        printf("Pingowanie klientow\n");
        int i = 0;
        while (i < MAX_NUMBER_OF_CLIENTS)
        {
            if (server_clients_arr[i].state != EMPTY)
            {
                if (server_clients_arr[i].respond)
                {
                    server_clients_arr[i].respond = false;
                    write(server_clients_arr[i].fd, &mess, sizeof(mess));
                }
                else
                {
                    delete_client(&server_clients_arr[i]);
                }
            }
            i++;
        }
        pthread_mutex_unlock(&mutex);
    }
}


void handle_sigint()
{
    printf("STOP\n");
    for (int i = 0; i < MAX_NUMBER_OF_CLIENTS; i++)
    {
        if (server_clients_arr[i].state != EMPTY)
        {
            send_end(&server_clients_arr[i], STOP, "");
            delete_client(&server_clients_arr[i]);
        }
    }
    shutdown(descriptor_epoll, SHUT_RDWR);
    close(descriptor_epoll);
    exit(0);
}


void initialize_server(int port, char *socket_path)
{
    descriptor_epoll = epoll_create1(0);
    struct sockaddr_un unix_addr;
    unix_addr.sun_family = AF_UNIX;
    strncpy(unix_addr.sun_path, socket_path, sizeof(unix_addr.sun_path));
    unlink(socket_path);
    int local_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    make_socket(local_sock, &unix_addr, sizeof(unix_addr));
    struct sockaddr_in inet_addr;
    memset(&inet_addr, 0, sizeof(inet_addr));
    inet_addr.sin_family = AF_INET;
    inet_addr.sin_port = htons(port);
    inet_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int web_sock = socket(AF_INET, SOCK_STREAM, 0);
    make_socket(web_sock, (struct sockaddr *)&inet_addr, sizeof(inet_addr));
    pthread_t ping_thread;
    pthread_create(&ping_thread, NULL, pinging, NULL);
    signal(SIGINT, handle_sigint);
}


void run_server()
{
    struct epoll_event events[1028];
    while (true)
    {
        int nread = epoll_wait(descriptor_epoll, events, 1028, 10);
        for (int i = 0; i < nread; i++)
        {
            event_data *data = events[i].data.ptr;
            if (data->type == SOCKET)
            {
                handle_socket_event(data);
            }
            else if (data->type == CLIENT)
            {
                handle_client_event(data, &events[i]);
            }
        }
    }
}


void handle_socket_event(event_data *data)
{
    int client_fd = accept(data->data.socket, NULL, NULL);
    client *client = handle_client(client_fd);
    if (client == NULL)
    {
        printf("Serwer jest pelny\n");
        MESSAGE mess;
        mess.type = FULL;
        write(client_fd, &mess, sizeof(mess));
        close(client_fd);
        return;
    }
    event_data *event_data_ptr = malloc(sizeof(event_data));
    event_data_ptr->type = CLIENT;
    event_data_ptr->data.client = client;
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.ptr = event_data_ptr;
    epoll_ctl(descriptor_epoll, EPOLL_CTL_ADD, client_fd, &event);
}


void handle_client_event(event_data *data, struct epoll_event *event)
{
    if (event->events & EPOLLHUP)
    {
        pthread_mutex_lock(&mutex);
        delete_client(data->data.client);
        pthread_mutex_unlock(&mutex);
    }
    else
    {
        message_to_client(data->data.client);
    }
}


int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Zla liczba argomentow\n");
        exit(0);
    }
    int port = atoi(argv[1]);
    char *socket_path = argv[2];
    initialize_server(port, socket_path);
    printf("Nasluchiwanie na porcie: %d sciezka unix: '%s'\n", port, socket_path);
    run_server();
    return 0;
}



