#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <mastik/list.h>

void traverse_list_skylake          (node *ptr);
void traverse_list_haswell          (node *ptr);
void traverse_list_simple           (node *ptr);
void traverse_list_asm_skylake      (node *ptr);
void traverse_list_asm_haswell      (node *ptr);
void traverse_list_asm_simple       (node *ptr);
void traverse_list_to_n             (node *ptr, int n);
void traverse_list_to_n_skylake     (node *ptr, int n);
void traverse_naive                 (node *ptr);
void traverse_flush                 (node *ptr);
void traverse_zigzag_victim         (node *ptr, void *victim);
