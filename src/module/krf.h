#pragma once

int control_file_handler(unsigned int syscall);
void targeting_file_read_handler(char *buf);
int targeting_file_write_handler(unsigned int mode, unsigned int data);
void krf_flush_table(void);
