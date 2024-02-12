#ifndef INTERFACE_H_
#define INTERFACE_H_

class Interface
{
private:
public:
    Interface();
    virtual ~Interface();

    virtual bool mqtt_publish(char *topic, unsigned char *msg, size_t msg_length) = 0;
    virtual bool mqtt_publish(unsigned char *msg, size_t msg_length, int slot) = 0;
};

#endif