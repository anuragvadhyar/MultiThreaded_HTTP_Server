#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <dirent.h>
#include "map.h"
#include "html_formatting.h"
#include "thread_pool.h"


char ** display_dir(int * size_arr)
{
    int no_files = 0;
    char ** fin = (char **)malloc(20 * sizeof(char *));
    for(int i = 0; i < 20; i++)
    {
        fin[i] = (char *)malloc(sizeof(char) * 100);
    }
    DIR * d;
    struct dirent *dir;
    d = opendir(".");
    
    if(d)
    {
        while((dir = readdir(d)) != NULL)
        {
            
            char * str_copy = strdup(dir->d_name);
            char *tok = NULL;
            tok = strtok(str_copy, ".");
            while(tok != NULL)
            {
                str_copy = tok;
                tok = strtok(NULL, ".");
            }
            if(strcmp("html", str_copy) == 0)
            {
                fin[no_files++] = dir->d_name;
            }
            str_copy = NULL;
        }
        closedir(d);
    }
    else
        {
            perror("dir error!");
        }
    *size_arr = no_files;
    return fin;

}

int handle_client(int client_fd, TreeNode * root) // task that the thread should execute
{
    char buffer[100];
    int rd;
    rd = read(client_fd, buffer, 99);
    if(rd == -1)
    {
        perror("read error!");
        return -1;
    }

    buffer[rd] = '\0';
    printf("%d bytes read and Buffer contents for client %d are:\n\n%s\n",rd, client_fd, buffer);

    int v;
    char request[100] = {0};

    for (v = 0; buffer[v] != '\n'; v++) {
        *(request + v) = buffer[v];
    }

    *(request+v) = '\0';
    printf("Request is:\n\n%s\n\n", request);

    char temp[100] = {0};
    char *check[5]={0};
    int check_p = 0;
    int x=0;

    for(v = 0;request[v] != '\0';v++)
    {
        if(request[v] == ' ')
        {
            temp[x] = '\0';
            check[check_p] = strdup(temp);
            check_p++;
            memset(temp, 0, 100);
            x=0;
        }
        else
        {
            *(temp + x) = request[v];
            x++;
        }
        
    }
    *(temp + x) = '\0';
    check[check_p] = strdup(temp);
    check_p++;
    if(check[1])
    {
        printf("%s\n\n", (char *)((check[1])+1)); //file directory to send contents of to client
        char * path_to_read = find_route(root,(char *)((check[1])+1));
        if(path_to_read == NULL)
        {
            char **fin = (char **)malloc(20 * sizeof(char *));
            for(int i = 0; i < 20; i++)
            {
                fin[i] = (char *)malloc(sizeof(char) * 255);
            }
            int size_arr = 0;
            fin = display_dir(&size_arr);
            write(client_fd, "Invalid route!Valid routes are:\n", 32);
            all_routes(&client_fd, root);
            
            // for(int j = 0; j < size_arr;j++)
            // {
            //     fin[j][strlen(fin[j])] = '\n';
            //     write(client_fd, fin[j], strlen(fin[j]));
            // }
            
            return 0;
        }
        printf("%s\n", path_to_read);
        FILE* fd_to_read;
        fd_to_read = fopen(path_to_read, "r");
        if(!fd_to_read)
        {
            perror("file open error!");
            return -1;
        }
        if(fseek(fd_to_read, 0, SEEK_END) == -1)
        {
            perror("file seek error!");
            return -1;
        }
        int file_length = ftell(fd_to_read);
        printf("file length is %d\n\n", file_length);
        if(fseek(fd_to_read, 0 , SEEK_SET) == -1)
        {
            perror("file seek error!");
            return -1;
        }

        char *temp_buffer= malloc(sizeof(char) * 1000);
        int buffer_size = 1000;
        int point;
        int index = 0;
        while((point = fgetc(fd_to_read)) != EOF)
        {
            if(index >= buffer_size - 1)
            {
                printf("in realloc");
                buffer_size *= 2;
                temp_buffer = (char *) realloc((void *)temp_buffer, buffer_size);
                if(temp_buffer == NULL)
                {
                    perror("Memory reallocation failed\n");
                    free(temp_buffer);
                    temp_buffer = NULL;
                    fclose(fd_to_read);
                    return -1;
                }
            }
            temp_buffer[index++] = (char) point;
        }
        temp_buffer[index] = '\0';
        fclose(fd_to_read);

        char * temp_ptr = malloc(sizeof(char)*(buffer_size+1000));
        int tot_char = html_format(temp_ptr, temp_buffer, file_length);

        // Send the response
        int wr;
        wr = write(client_fd, temp_ptr, tot_char);
        if(wr == -1)
        {
            perror("write error!");
            return -1;
        }
        free(temp_buffer);
        free(temp_ptr);
        temp_buffer = NULL;
        temp_ptr = NULL;
        //break;
    }
    return 0;
}
int main()
{
    pthread_mutex_init(&mutexQueue, NULL);
    pthread_cond_init(&condQueue, NULL);
    pthread_t thread_arr[THREAD_NUM];
    struct sockaddr_in addr;
    int fd;
    fd = socket(AF_INET, SOCK_STREAM, 0); 
    struct sockaddr_in peer_addr;
    if(fd == -1)
    {
        perror("Socket init error");
        return -1;
    }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(80);

    int res;
    res = bind(fd, (struct sockaddr *) &addr,
                sizeof(addr));
    if(res == -1)
    {
        perror("Cannot bind!");
        return -1;
    }

    res = listen(fd, 5);
    if(res == -1)
    {
        perror("listening error");
        return -1;
    }
    uint32_t peer_size = sizeof(peer_addr);
    TreeNode *root = add_route(NULL, "root", "pages/index.html");
    // root = add_route(NULL, "root", "index.html");
    root = add_route(root, "test", "pages/test_2.html");
    // all_routes(root); //Print all routes

    for(int i = 0; i < THREAD_NUM; i++)
    {
        int *arg = malloc(sizeof(int));
        *arg = i;
        if(pthread_create(&thread_arr[i], NULL, &start_thread, arg) != 0)
        {
            perror("Failed to create thread");
        }
    }

    while((res = accept(fd,(struct sockaddr *) &peer_addr, &peer_size)) != -1) // main thread to create and manage threads and create and insert tasks
    {
        printf("res is %d\n\n", res);
        if(res == -1)
        {
            perror("accept error");
            return -1;
        }
        Task t = {
            .client_fd = res,
            .root = root,
            .task_func = handle_client

        };
        submitTask(t);
        // int close_fd;
        // close_fd = close(res);
        // if(close_fd == -1)
        // {
        //     perror("close error!");
        //     return -1;
        // }

    }
    for(int j = 0;j < THREAD_NUM; j++)
    {
        if(pthread_join(thread_arr[j], NULL) != 0)
        {
            perror("thread join failed!");
            return -1;
        }
    }
    int close_fd;
    close_fd = close(fd);
    if(close_fd == -1)
    {
        perror("close error!");
        return -1;
    }
    printf("Works till here.");
    return 0;
}

// GET /hello HTTP/1.1
// Host: 0.0.0.0
// User-Agent: curl/8.7.1
// Accept: */*