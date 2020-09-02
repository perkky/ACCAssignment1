#pragma once
#include "history.h"
#include <semaphore.h>
//Global variables

FileHistory* g_history;
sem_t* history_sem;
