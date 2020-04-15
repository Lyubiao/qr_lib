/*
 * @Descripttion: 
 * @version: 
 * @Author: Lyubiao
 * @Github: https://github.com/Lyubiao
 * @Date: 2019-11-08 16:08:41
 * @LastEditors: 
 * @LastEditTime: 2019-11-08 21:03:06
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BUFF_IS_VALID(rb) ((rb) != NULL && (rb)->buffer != NULL && (rb)->size > 0)

#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define MAX(x, y) ((x) > (y) ? (x) : (y))

typedef struct _rb
{
    int size;
    unsigned char *read;
    unsigned char *write;
    unsigned char *buffer;
} rb_t;

/**
 * @brief: 初始化rb 
 * @param1: self  rb_t结构体指针
 * @param2: buffer 队列缓存的地址
 * @param3: size 队列长度
 * @return: -1：参数错误，1：初始化王完成
 */
int rb_init(rb_t *self, unsigned char *buffer, int size)
{
    if (self == NULL || buffer == NULL)
    {
        return -1;
    }
    self->size = size;
    self->buffer = buffer;
    self->read = self->buffer;
    self->write = self->buffer;
    return 1;
}
/**
 * @brief:  删除队列
 * @param1: self rb_t结构体指针
 * @return: -1：参数错误，1删除完成
 */
int rb_del(rb_t *self)
{
    if (self == NULL)
    {
        return -1;
    }
    self->size = 0;
    self->buffer = NULL;
    self->read = NULL;
    self->write = NULL;
    return 1;
}

/**
 * @brief: 获取队列剩余的长度
 * @param1: self rb_t结构体指针
 * @return: -1：参数错误， 其他：剩余的数据长度
 */
int rb_get_free(rb_t *self)
{
    int size;
    if (self == NULL)
    {
        return -1;
    }
    if (self->write >= self->read)
    {
        size = self->size + (self->read - self->write);
    }
    else
    {
        size = self->read - self->write;
    }
    return size - 1; //减一是因为为了区分空和满占用了一个元素。
}

/**
 * @brief: 获取队列已写数据的长度
 * @param1: self rb_t结构体指针
 * @return: -1：参数错误，其他：已覆盖的数据长度
 */
int rb_get_full(rb_t *rb)
{
    int size;
    if (rb == NULL)
    {
        return -1;
    }
    if (rb->write >= rb->read)
    {

        size = rb->write - rb->read;
    }
    else
    {
        size = rb->size + (rb->write - rb->read);
    }
    return size;
}

int rb_is_empty(rb_t *rb)
{
    if (rb == NULL)
    {
        return -1;
    }
    if (rb_get_full(rb) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int rb_is_full(rb_t *rb)
{
    if (rb == NULL)
    {
        return -1;
    }
    if (rb_get_free(rb) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int rb_write(rb_t *rb, void *data, int write_length)
{
    int free_length, tocopy;
    if (!BUFF_IS_VALID(rb))
    {
        return -1;
    }
    free_length = rb_get_free(rb);
    write_length = MIN(free_length, write_length); //超过的数据直接舍弃
    if (rb->write >= rb->read)
    {
        tocopy = rb->buffer + rb->size - rb->write;
        if (write_length <= tocopy)
        {
            memcpy(rb->write, data, sizeof(unsigned char) * write_length);
            rb->write += write_length;
            if (rb->write == rb->buffer + rb->size)
            {
                rb->write = rb->buffer;
            }
        }
        else
        {
            memcpy(rb->write, data, sizeof(unsigned char) * tocopy);
            rb->write = rb->buffer;
            memcpy(rb->write, (unsigned char *)data + tocopy, sizeof(unsigned char) * (write_length - tocopy));
            rb->write += (write_length - tocopy);
        }
    }
    else
    {
        memcpy(rb->write, data, sizeof(unsigned char) * write_length);
        rb->write += write_length;
    }
    return write_length;
}
int rb_read(rb_t *rb, void *data, int read_length)
{
    int full_length, tocopy;
    if (!BUFF_IS_VALID(rb))
    {
        return -1;
    }
    full_length = rb_get_full(rb);
    read_length = MIN(read_length, full_length);

    if (rb->write < rb->read)
    {
        tocopy = rb->buffer + rb->size - rb->read;
        if (read_length <= tocopy)
        {
            memcpy(rb->write, data, sizeof(unsigned char) * read_length);
            rb->read += read_length;
            if (rb->read == rb->buffer + rb->size)
            {
                rb->read = rb->buffer;
            }
        }
        else
        {
            memcpy(data, rb->read, tocopy);
            rb->read = rb->buffer;
            memcpy((unsigned char *)data + tocopy, rb->read, sizeof(unsigned char) * (read_length - tocopy));
            rb->read += (read_length - tocopy);
        }
    }
    else
    {
        memcpy(data, rb->read, sizeof(unsigned char) * read_length);
        rb->read += read_length;
    }
    return read_length;
}

int main()
{

    int size;
    rb_t test;
    unsigned char send[10] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'};
    unsigned char recv[10] = {0};
    unsigned char test_array[10] = {0};
    rb_init(&test, test_array, sizeof(test_array) / sizeof(unsigned char));
    size = rb_write(&test, send, 7);
    printf("write_size = %d\r\n", size);
    size = rb_write(&test, &send[5], 4);
    printf("write_size = %d\r\n", size);
    printf("-------------------------\r\n");
    size = rb_read(&test, recv, 4);
    printf("read_size = %d\r\n", size);
    for (int i = 0; i < size; i++)
    {
        printf("%c\r\n", recv[i]);
    }
    printf("-------------------------\r\n");
    size = rb_read(&test, recv, 3);
    printf("read_size = %d\r\n", size);
    for (int i = 0; i < size; i++)
    {
        printf("%c\r\n", recv[i]);
    }
    printf("-------------------------\r\n");
    size = rb_read(&test, recv, 100);
    printf("read_size = %d\r\n", size);
    for (int i = 0; i < size; i++)
    {
        printf("%c\r\n", recv[i]);
    }
}