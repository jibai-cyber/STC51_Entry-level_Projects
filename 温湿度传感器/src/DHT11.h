#ifndef __DHT11_H__
#define __DHT11_H__
void DHT11_delay_us(unsigned char n);
void DHT11_delay_ms(unsigned char z);
void DHT11_start();
unsigned char DHT11_rec_byte();
void DHT11_receive(unsigned char*R,unsigned char*T);


#endif