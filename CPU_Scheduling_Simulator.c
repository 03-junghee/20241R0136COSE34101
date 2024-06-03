#pragma warning(disable:4996)
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

int Rank_Waiting_Time[6] = { 0, 1, 2, 3, 4, 5 };
int Rank_Turnaround_Time[6] = { 0, 1, 2, 3, 4, 5 };
int Rank_CPU_Utilization[6] = { 0, 1, 2, 3, 4, 5 };
float Average_Waiting_Time[6], Average_Turnaround_Time[6], CPU_Utilization[6];
int left_IO[6][5];

typedef struct {
	int Process_ID;
	int CPU_Burst_Time;
	int IO_Burst_Time;
	int Arrival_Time;
	int Priority;
	int IO_Burst_Timing;
}Process;

typedef struct {
	int* ID;
	int cnt;
}QUEUE;

QUEUE* Create_Queue(void) {
	QUEUE* buf = (QUEUE*)malloc(sizeof(QUEUE));
	buf->ID = (int*)malloc(sizeof(int) * 1000);
	buf->cnt = 0;
	return buf;
}

void Enqueue(QUEUE* queue, int name) {
	*(queue->ID + (queue->cnt)) = name;
	queue->cnt++;
}

void Dequeue(QUEUE* queue) {
	queue->cnt--;
	memmove(queue->ID, queue->ID + 1, sizeof(int) * (queue->cnt));
}

Process Create_Process(int ID) {
	Process buf;

	buf.Process_ID = ID;
	buf.CPU_Burst_Time = rand() % 20 + 1;
	buf.IO_Burst_Time = rand() % 5 + 1;
	buf.Arrival_Time = rand() % 20 + 1;
	buf.Priority = rand() % 10 + 1;
	buf.IO_Burst_Timing = rand() % buf.CPU_Burst_Time;

	return buf;
}

Process Copy_Process(Process process) {
	Process buf;

	buf.Process_ID = process.Process_ID;
	buf.CPU_Burst_Time = process.CPU_Burst_Time;
	buf.IO_Burst_Time = process.IO_Burst_Time;
	buf.Arrival_Time = process.Arrival_Time;
	buf.Priority = process.Priority;
	buf.IO_Burst_Timing = process.IO_Burst_Timing;

	return buf;
}

void Print_Process(Process process_Info[], int Process_Cnt) {

	for (int i = 0; i < Process_Cnt; i++) {
		printf("Process %d: \n", i + 1);
		printf("CPU_Burst_Time = %d\n", process_Info[i].CPU_Burst_Time);
		printf("IO_Burst_Time = %d\n", process_Info[i].IO_Burst_Time);
		printf("Arrival_Time = %d\n", process_Info[i].Arrival_Time);
		printf("Priority = %d\n", process_Info[i].Priority);
		printf("IO_Burst_Timing = %d\n", process_Info[i].IO_Burst_Timing);
		printf("\n");
	}
}

void Print_Gantt_Chart(int** CPU_Info, int flag) {
	for (int i = 0; i < flag; i++) printf("|      ");
	printf("|\n");
	for (int i = 0; i < flag; i++) {
		if (CPU_Info[1][i] == -1) printf("| IDLE ");
		else printf("|  P%d  ", CPU_Info[1][i]);
	}
	printf("|\n");
	for (int i = 0; i < flag; i++) printf("|      ");
	printf("|\n");
	printf("0      ");
	for (int i = 0; i < flag; i++) {
		if (CPU_Info[0][i] < 10) printf("%d      ", CPU_Info[0][i]);
		else printf("%d     ", CPU_Info[0][i]);
	}

	printf("\n\n");
}

float Calculate_Average_Waiting_Time(Process Process_Info[], int Process_Cnt, int** CPU_Info, int flag, int left_IO_flag) {
	int** Keep_Value = (int**)malloc(sizeof(int*) * 2);
	int Total = 0;
	int Future_Time = 0;
	int Currunt_Time = 0;

	for (int i = 0; i < 2; i++) Keep_Value[i] = (int*)malloc(sizeof(int) * Process_Cnt);
	for (int i = 0; i < Process_Cnt; i++) {
		Keep_Value[0][i] = 0;
		Keep_Value[1][i] = 0;
	}
	for (int i = 0; i < flag; i++) {
		if (i == 0) Currunt_Time = 0;
		else Currunt_Time = CPU_Info[0][i - 1];
		Future_Time = CPU_Info[0][i];
		if (CPU_Info[1][i] != -1) {
			if (Keep_Value[1][CPU_Info[1][i] - 1] == 0) {
				Keep_Value[0][CPU_Info[1][i] - 1] = Currunt_Time - Process_Info[CPU_Info[1][i] - 1].Arrival_Time;
				Keep_Value[1][CPU_Info[1][i] - 1] = Future_Time;
			}
			else {
				Keep_Value[0][CPU_Info[1][i] - 1] += (Currunt_Time - Keep_Value[1][CPU_Info[1][i] - 1]);
				Keep_Value[1][CPU_Info[1][i] - 1] = Future_Time;
			}
		}
	}
	for (int i = 0; i < Process_Cnt; i++) {
		if(Process_Info[i].IO_Burst_Timing > 0) Keep_Value[0][i] -= (Process_Info[i].IO_Burst_Time + left_IO[left_IO_flag][i]);
	}
	printf("Average Waiting Time :\n");
	printf("( ");
	for (int i = 0; i < Process_Cnt; i++) {
		if (i != Process_Cnt - 1) printf("%d + ", Keep_Value[0][i]);
		else printf("%d )", Keep_Value[0][i]);
		Total += Keep_Value[0][i];
	}

	float ans = (float)Total / Process_Cnt;

	printf(" / %d = %.1f\n\n", Process_Cnt, ans);
	return ans;
}

float Calculate_Average_Turnaround_Time(Process Process_Info[], int Process_Cnt, int** CPU_Info, int flag) {
	int* Keep_Value = (int*)malloc(sizeof(int) * Process_Cnt);
	int Total = 0;

	for (int i = 0; i < Process_Cnt; i++) Keep_Value[i] = 0;

	for (int i = 0; i < flag; i++) if (CPU_Info[1][i] != -1) Keep_Value[CPU_Info[1][i] - 1] = CPU_Info[0][i];

	for (int i = 0; i < Process_Cnt; i++) Keep_Value[i] -= Process_Info[i].Arrival_Time;

	//for (int i = 0; i < Process_Cnt; i++) Keep_Value[i] -= Process_Info[i].IO_Burst_Time;

	printf("Average Turnaround Time :\n");
	printf("( ");
	for (int i = 0; i < Process_Cnt; i++) {
		if (i != Process_Cnt - 1) printf("%d + ", Keep_Value[i]);
		else printf("%d )", Keep_Value[i]);
		Total += Keep_Value[i];
	}

	float ans = (float)Total / Process_Cnt;

	printf(" / %d = %.1f\n\n", Process_Cnt, ans);
	return ans;
}

float Calculate_CPU_Utilization(int** CPU_Info, int flag) {
	int CPU_Idle = 0;
	int Last_Time = CPU_Info[0][flag - 1];
	for (int i = 0; i < flag; i++) {
		if (CPU_Info[1][i] == -1) {
			if (i == 0) CPU_Idle += CPU_Info[0][i];
			else CPU_Idle += CPU_Info[0][i] - CPU_Info[0][i - 1];
		}
	}

	int CPU_Not_Idle = Last_Time - CPU_Idle;
	float ans = (float)CPU_Not_Idle / Last_Time;

	printf("CPU Utilization :\n");
	printf("%d / %d = %.0f%%\n\n", CPU_Not_Idle, Last_Time, ans * 100);

	return ans;
}

void FCFS(Process Process_Info[], int Process_Cnt) {
	QUEUE* Ready_Queue = Create_Queue();
	QUEUE* Waiting_Queue = Create_Queue();

	Process* process = (Process*)malloc(sizeof(Process) * Process_Cnt);
	for (int i = 0; i < Process_Cnt; i++) process[i] = Copy_Process(Process_Info[i]);

	int Process_End_Cnt = 0;
	int flag = 0;

	int** CPU_Info = (int**)malloc(sizeof(int) * 2);
	for (int i = 0; i < 2; i++) CPU_Info[i] = (int*)malloc(sizeof(int) * 500);

	int Currunt_CPU_Process = -1;
	int Currunt_IO_Process = -1;
	int CPU_Idle_Time = 0;
	int CPU_Time = 0;

	while (Process_End_Cnt < Process_Cnt) {
		for (int i = 0; i < Process_Cnt; i++) if (CPU_Time == process[i].Arrival_Time) Enqueue(Ready_Queue, i + 1);

		CPU_Time++;

		if (Currunt_CPU_Process == -1 && Ready_Queue->cnt == 0) CPU_Idle_Time++;
		else {
			if (Currunt_CPU_Process == -1) {
				if (CPU_Idle_Time != 0) {
					CPU_Info[0][flag] = CPU_Time - 1;
					CPU_Info[1][flag] = -1;
					flag++;
					CPU_Idle_Time = 0;
				}
				Currunt_CPU_Process = Ready_Queue->ID[0];
				Dequeue(Ready_Queue);
			}
			process[Currunt_CPU_Process - 1].CPU_Burst_Time--;
			if (process[Currunt_CPU_Process - 1].CPU_Burst_Time == 0) {
				CPU_Info[0][flag] = CPU_Time;
				CPU_Info[1][flag] = Currunt_CPU_Process;
				flag++;
				Currunt_CPU_Process = -1;
				Process_End_Cnt++;
			}
			if (process[Currunt_CPU_Process - 1].CPU_Burst_Time == process[Currunt_CPU_Process - 1].IO_Burst_Timing) {
				left_IO[0][Currunt_CPU_Process - 1] = -CPU_Time;
				Enqueue(Waiting_Queue, Currunt_CPU_Process);
				CPU_Info[0][flag] = CPU_Time;
				CPU_Info[1][flag] = Currunt_CPU_Process;
				flag++;
				Currunt_CPU_Process = -1;
			}
		}

		if (Currunt_IO_Process != -1) process[Currunt_IO_Process - 1].IO_Burst_Time--;
		if (Currunt_IO_Process != -1 && process[Currunt_IO_Process - 1].IO_Burst_Time == 0) {
			if(process[Currunt_IO_Process - 1].CPU_Burst_Time != 0) Enqueue(Ready_Queue, Currunt_IO_Process);
			Currunt_IO_Process = -1;
		}
		if (Currunt_IO_Process == -1 && Waiting_Queue->cnt != 0) {
			Currunt_IO_Process = Waiting_Queue->ID[0];
			left_IO[0][Currunt_IO_Process - 1] += CPU_Time;
			Dequeue(Waiting_Queue);
		}

	}

	printf("FCFS:\n");
	Print_Gantt_Chart(CPU_Info, flag);

	float FCFS_Average_Waiting_Time = Calculate_Average_Waiting_Time(Process_Info, Process_Cnt, CPU_Info, flag, 0);
	float FCFS_Average_Turnaround_Time = Calculate_Average_Turnaround_Time(Process_Info, Process_Cnt, CPU_Info, flag);
	float FCFS_CPU_Utilization = Calculate_CPU_Utilization(CPU_Info, flag);

	Average_Waiting_Time[0] = FCFS_Average_Waiting_Time;
	Average_Turnaround_Time[0] = FCFS_Average_Turnaround_Time;
	CPU_Utilization[0] = FCFS_CPU_Utilization;

	/*
	free(Ready_Queue->ID);
	free(Waiting_Queue->ID);
	free(Ready_Queue);
	free(Waiting_Queue);
	free(process);
	for (int i = 0; i < 2; i++) free(CPU_Info[i]);
	free(CPU_Info);
	*/
}

void SJF(Process Process_Info[], int Process_Cnt) {
	QUEUE* Ready_Queue = Create_Queue();
	QUEUE* Waiting_Queue = Create_Queue();

	Process* process = (Process*)malloc(sizeof(Process) * Process_Cnt);
	for (int i = 0; i < Process_Cnt; i++) process[i] = Copy_Process(Process_Info[i]);

	int Process_End_Cnt = 0;
	int flag = 0;

	int** CPU_Info = (int**)malloc(sizeof(int) * 2);
	for (int i = 0; i < 2; i++) CPU_Info[i] = (int*)malloc(sizeof(int) * 500);

	int Currunt_CPU_Process = -1;
	int Currunt_IO_Process = -1;
	int CPU_Idle_Time = 0;
	int CPU_Time = 0;

	while (Process_End_Cnt < Process_Cnt) {
		for (int i = 0; i < Process_Cnt; i++) if (CPU_Time == process[i].Arrival_Time) {
			Enqueue(Ready_Queue, i + 1);
			for (int j = 0; j < Ready_Queue->cnt; j++) {
				for (int k = j + 1; k < Ready_Queue->cnt; k++) {
					if (process[Ready_Queue->ID[j] - 1].CPU_Burst_Time > process[Ready_Queue->ID[k] - 1].CPU_Burst_Time) {
						int temp = Ready_Queue->ID[j];
						Ready_Queue->ID[j] = Ready_Queue->ID[k];
						Ready_Queue->ID[k] = temp;
					}
				}
			}
		}

		CPU_Time++;

		if (Currunt_CPU_Process == -1 && Ready_Queue->cnt == 0) CPU_Idle_Time++;
		else {
			if (Currunt_CPU_Process == -1) {
				if (CPU_Idle_Time != 0) {
					CPU_Info[0][flag] = CPU_Time - 1;
					CPU_Info[1][flag] = -1;
					flag++;
					CPU_Idle_Time = 0;
				}
				Currunt_CPU_Process = Ready_Queue->ID[0];
				Dequeue(Ready_Queue);
			}
			process[Currunt_CPU_Process - 1].CPU_Burst_Time--;
			if (process[Currunt_CPU_Process - 1].CPU_Burst_Time == 0) {
				CPU_Info[0][flag] = CPU_Time;
				CPU_Info[1][flag] = Currunt_CPU_Process;
				flag++;
				if (process[Currunt_CPU_Process - 1].CPU_Burst_Time == process[Currunt_CPU_Process - 1].IO_Burst_Timing) {
					left_IO[1][Currunt_CPU_Process - 1] = -CPU_Time;
					Enqueue(Waiting_Queue, Currunt_CPU_Process);
				}
				Currunt_CPU_Process = -1;
				Process_End_Cnt++;
			}
			if (process[Currunt_CPU_Process - 1].CPU_Burst_Time == process[Currunt_CPU_Process - 1].IO_Burst_Timing) {
				left_IO[1][Currunt_CPU_Process - 1] = -CPU_Time;
				Enqueue(Waiting_Queue, Currunt_CPU_Process);
				CPU_Info[0][flag] = CPU_Time;
				CPU_Info[1][flag] = Currunt_CPU_Process;
				flag++;
				Currunt_CPU_Process = -1;
			}
		}

		if (Currunt_IO_Process != -1) process[Currunt_IO_Process - 1].IO_Burst_Time--;
		if (Currunt_IO_Process != -1 && process[Currunt_IO_Process - 1].IO_Burst_Time == 0) {
			if (process[Currunt_IO_Process - 1].CPU_Burst_Time > 0) {
				Enqueue(Ready_Queue, Currunt_IO_Process);
				for (int i = 0; i < Ready_Queue->cnt; i++) {
					for (int j = i + 1; j < Ready_Queue->cnt; j++) {
						if (process[Ready_Queue->ID[i] - 1].CPU_Burst_Time > process[Ready_Queue->ID[j] - 1].CPU_Burst_Time) {
							int temp = Ready_Queue->ID[i];
							Ready_Queue->ID[i] = Ready_Queue->ID[j];
							Ready_Queue->ID[j] = temp;
						}
					}
				}
			}
			Currunt_IO_Process = -1;
		}
		if (Currunt_IO_Process == -1 && Waiting_Queue->cnt > 0) {
			Currunt_IO_Process = Waiting_Queue->ID[0];
			left_IO[1][Currunt_IO_Process - 1] += CPU_Time;
			Dequeue(Waiting_Queue);
		}
	}

	printf("SJF:\n");
	Print_Gantt_Chart(CPU_Info, flag);

	float SJF_Average_Waiting_Time = Calculate_Average_Waiting_Time(Process_Info, Process_Cnt, CPU_Info, flag, 1);
	float SJF_Average_Turnaround_Time = Calculate_Average_Turnaround_Time(Process_Info, Process_Cnt, CPU_Info, flag);
	float SJF_CPU_Utilization = Calculate_CPU_Utilization(CPU_Info, flag);

	Average_Waiting_Time[1] = SJF_Average_Waiting_Time;
	Average_Turnaround_Time[1] = SJF_Average_Turnaround_Time;
	CPU_Utilization[1] = SJF_CPU_Utilization;

	/*
	free(Ready_Queue->ID);
	free(Waiting_Queue->ID);
	free(Ready_Queue);
	free(Waiting_Queue);
	free(process);
	for (int i = 0; i < 2; i++) free(CPU_Info[i]);
	free(CPU_Info);
	*/
}

void Priority(Process Process_Info[], int Process_Cnt) {
	QUEUE* Ready_Queue = Create_Queue();
	QUEUE* Waiting_Queue = Create_Queue();

	Process* process = (Process*)malloc(sizeof(Process) * Process_Cnt);
	for (int i = 0; i < Process_Cnt; i++) process[i] = Copy_Process(Process_Info[i]);

	int Process_End_Cnt = 0;
	int flag = 0;

	int** CPU_Info = (int**)malloc(sizeof(int) * 2);
	for (int i = 0; i < 2; i++) CPU_Info[i] = (int*)malloc(sizeof(int) * 500);

	int Currunt_CPU_Process = -1;
	int Currunt_IO_Process = -1;
	int CPU_Idle_Time = 0;
	int CPU_Time = 0;

	while (Process_End_Cnt < Process_Cnt) {
		for (int i = 0; i < Process_Cnt; i++) if (CPU_Time == process[i].Arrival_Time) {
			Enqueue(Ready_Queue, i + 1);
			for (int j = 0; j < Ready_Queue->cnt; j++) {
				for (int k = j + 1; k < Ready_Queue->cnt; k++) {
					if (process[Ready_Queue->ID[j] - 1].Priority < process[Ready_Queue->ID[k] - 1].Priority) {
						int temp = Ready_Queue->ID[j];
						Ready_Queue->ID[j] = Ready_Queue->ID[k];
						Ready_Queue->ID[k] = temp;
					}
				}
			}
		}

		CPU_Time++;

		if (Currunt_CPU_Process == -1 && Ready_Queue->cnt == 0) CPU_Idle_Time++;
		else {
			if (Currunt_CPU_Process == -1) {
				if (CPU_Idle_Time != 0) {
					CPU_Info[0][flag] = CPU_Time - 1;
					CPU_Info[1][flag] = -1;
					flag++;
					CPU_Idle_Time = 0;
				}
				Currunt_CPU_Process = Ready_Queue->ID[0];
				Dequeue(Ready_Queue);
			}
			process[Currunt_CPU_Process - 1].CPU_Burst_Time--;
			if (process[Currunt_CPU_Process - 1].CPU_Burst_Time == 0) {
				CPU_Info[0][flag] = CPU_Time;
				CPU_Info[1][flag] = Currunt_CPU_Process;
				flag++;
				if (process[Currunt_CPU_Process - 1].CPU_Burst_Time == process[Currunt_CPU_Process - 1].IO_Burst_Timing) {
					left_IO[2][Currunt_CPU_Process - 1] = -CPU_Time;
					Enqueue(Waiting_Queue, Currunt_CPU_Process);
				}
				Currunt_CPU_Process = -1;
				Process_End_Cnt++;
			}
			if (process[Currunt_CPU_Process - 1].CPU_Burst_Time == process[Currunt_CPU_Process - 1].IO_Burst_Timing) {
				left_IO[2][Currunt_CPU_Process - 1] = -CPU_Time;
				Enqueue(Waiting_Queue, Currunt_CPU_Process);
				CPU_Info[0][flag] = CPU_Time;
				CPU_Info[1][flag] = Currunt_CPU_Process;
				flag++;
				Currunt_CPU_Process = -1;
			}
		}

		if (Currunt_IO_Process != -1) process[Currunt_IO_Process - 1].IO_Burst_Time--;
		if (Currunt_IO_Process != -1 && process[Currunt_IO_Process - 1].IO_Burst_Time == 0) {
			if (process[Currunt_IO_Process - 1].CPU_Burst_Time > 0) {
				Enqueue(Ready_Queue, Currunt_IO_Process);
				for (int i = 0; i < Ready_Queue->cnt; i++) {
					for (int j = i + 1; j < Ready_Queue->cnt; j++) {
						if (process[Ready_Queue->ID[i] - 1].Priority < process[Ready_Queue->ID[j] - 1].Priority) {
							int temp = Ready_Queue->ID[i];
							Ready_Queue->ID[i] = Ready_Queue->ID[j];
							Ready_Queue->ID[j] = temp;
						}
					}
				}
			}
			Currunt_IO_Process = -1;
		}
		if (Currunt_IO_Process == -1 && Waiting_Queue->cnt > 0) {
			Currunt_IO_Process = Waiting_Queue->ID[0];
			left_IO[2][Currunt_IO_Process - 1] += CPU_Time;
			Dequeue(Waiting_Queue);
		}
	}

	printf("Priority:\n");
	Print_Gantt_Chart(CPU_Info, flag);

	float Priority_Average_Waiting_Time = Calculate_Average_Waiting_Time(Process_Info, Process_Cnt, CPU_Info, flag, 2);
	float Priority_Average_Turnaround_Time = Calculate_Average_Turnaround_Time(Process_Info, Process_Cnt, CPU_Info, flag);
	float Priority_CPU_Utilization = Calculate_CPU_Utilization(CPU_Info, flag);

	Average_Waiting_Time[2] = Priority_Average_Waiting_Time;
	Average_Turnaround_Time[2] = Priority_Average_Turnaround_Time;
	CPU_Utilization[2] = Priority_CPU_Utilization;

	/*
	free(Ready_Queue->ID);
	free(Waiting_Queue->ID);
	free(Ready_Queue);
	free(Waiting_Queue);
	free(process);
	for (int i = 0; i < 2; i++) free(CPU_Info[i]);
	free(CPU_Info);
	*/
}

void RR(Process Process_Info[], int Process_Cnt, int quantum) {
	QUEUE* Ready_Queue = Create_Queue();
	QUEUE* Waiting_Queue = Create_Queue();

	Process* process = (Process*)malloc(sizeof(Process) * Process_Cnt);
	for (int i = 0; i < Process_Cnt; i++) process[i] = Copy_Process(Process_Info[i]);

	int Process_End_Cnt = 0;
	int flag = 0;

	int** CPU_Info = (int**)malloc(sizeof(int*) * 2);
	for (int i = 0; i < 2; i++) CPU_Info[i] = (int*)malloc(sizeof(int) * 500);

	int Currunt_CPU_Process = -1;
	int Currunt_IO_Process = -1;
	int CPU_Idle_Time = 0;
	int CPU_Time = 0;
	int Quantum_Time = 0;

	while (Process_End_Cnt < Process_Cnt) {
		for (int i = 0; i < Process_Cnt; i++) if (CPU_Time == process[i].Arrival_Time) Enqueue(Ready_Queue, i + 1);

		CPU_Time++;

		if (Currunt_CPU_Process == -1 && Ready_Queue->cnt == 0) CPU_Idle_Time++;
		else {
			if (Currunt_CPU_Process == -1) {
				if (CPU_Idle_Time != 0) {
					CPU_Info[0][flag] = CPU_Time - 1;
					CPU_Info[1][flag] = -1;
					flag++;
					CPU_Idle_Time = 0;
				}
				Currunt_CPU_Process = Ready_Queue->ID[0];
				Dequeue(Ready_Queue);
			}
			process[Currunt_CPU_Process - 1].CPU_Burst_Time--;
			Quantum_Time++;
			if (process[Currunt_CPU_Process - 1].CPU_Burst_Time == 0) {
				CPU_Info[0][flag] = CPU_Time;
				CPU_Info[1][flag] = Currunt_CPU_Process;
				flag++;
				Process_End_Cnt++;
				Quantum_Time = 0;
				Currunt_CPU_Process = -1;
			}
			else if (process[Currunt_CPU_Process - 1].CPU_Burst_Time == process[Currunt_CPU_Process - 1].IO_Burst_Timing) {
				left_IO[3][Currunt_CPU_Process - 1] = -CPU_Time;
				Enqueue(Waiting_Queue, Currunt_CPU_Process);
				CPU_Info[0][flag] = CPU_Time;
				CPU_Info[1][flag] = Currunt_CPU_Process;
				flag++;
				Quantum_Time = 0;
				Currunt_CPU_Process = -1;
			}
			else if (Quantum_Time == quantum) {
				CPU_Info[0][flag] = CPU_Time;
				CPU_Info[1][flag] = Currunt_CPU_Process;
				flag++;
				Enqueue(Ready_Queue, Currunt_CPU_Process);
				Quantum_Time = 0;
				Currunt_CPU_Process = -1;
			}
		}

		if (Currunt_IO_Process != -1) process[Currunt_IO_Process - 1].IO_Burst_Time--;
		if (Currunt_IO_Process != -1 && process[Currunt_IO_Process - 1].IO_Burst_Time == 0) {
			if (process[Currunt_IO_Process - 1].CPU_Burst_Time != 0) Enqueue(Ready_Queue, Currunt_IO_Process);
			Currunt_IO_Process = -1;
		}
		if (Currunt_IO_Process == -1 && Waiting_Queue->cnt != 0) {
			Currunt_IO_Process = Waiting_Queue->ID[0];
			left_IO[3][Currunt_IO_Process - 1] += CPU_Time;
			Dequeue(Waiting_Queue);
		}

	}

	printf("RR:\n");
	Print_Gantt_Chart(CPU_Info, flag);

	float RR_Average_Waiting_Time = Calculate_Average_Waiting_Time(Process_Info, Process_Cnt, CPU_Info, flag, 3);
	float RR_Average_Turnaround_Time = Calculate_Average_Turnaround_Time(Process_Info, Process_Cnt, CPU_Info, flag);
	float RR_CPU_Utilization = Calculate_CPU_Utilization(CPU_Info, flag);

	Average_Waiting_Time[3] = RR_Average_Waiting_Time;
	Average_Turnaround_Time[3] = RR_Average_Turnaround_Time;
	CPU_Utilization[3] = RR_CPU_Utilization;

	/*
	free(Ready_Queue->ID);
	free(Waiting_Queue->ID);
	free(Ready_Queue);
	free(Waiting_Queue);
	free(process);
	for (int i = 0; i < 2; i++) free(CPU_Info[i]);
	free(CPU_Info);
	*/
}

void P_SJF(Process Process_Info[], int Process_Cnt) {
	QUEUE* Ready_Queue = Create_Queue();
	QUEUE* Waiting_Queue = Create_Queue();

	Process* process = (Process*)malloc(sizeof(Process) * Process_Cnt);
	for (int i = 0; i < Process_Cnt; i++) process[i] = Copy_Process(Process_Info[i]);

	int Process_End_Cnt = 0;
	int flag = 0;

	int** CPU_Info = (int**)malloc(sizeof(int) * 2);
	for (int i = 0; i < 2; i++) CPU_Info[i] = (int*)malloc(sizeof(int) * 500);

	int Currunt_CPU_Process = -1;
	int Currunt_IO_Process = -1;
	int CPU_Idle_Time = 0;
	int CPU_Time = 0;

	while (Process_End_Cnt < Process_Cnt) {
		for (int i = 0; i < Process_Cnt; i++) if (CPU_Time == process[i].Arrival_Time) {
			Enqueue(Ready_Queue, i + 1);
			for (int j = 0; j < Ready_Queue->cnt; j++) {
				for (int k = j + 1; k < Ready_Queue->cnt; k++) {
					if (process[Ready_Queue->ID[j] - 1].CPU_Burst_Time > process[Ready_Queue->ID[k] - 1].CPU_Burst_Time) {
						int temp = Ready_Queue->ID[j];
						Ready_Queue->ID[j] = Ready_Queue->ID[k];
						Ready_Queue->ID[k] = temp;
					}
				}
			}
			if (Currunt_CPU_Process != -1 && process[Currunt_CPU_Process - 1].CPU_Burst_Time > process[Ready_Queue->ID[0] - 1].CPU_Burst_Time) {
				CPU_Info[0][flag] = CPU_Time;
				CPU_Info[1][flag] = Currunt_CPU_Process;
				flag++;
				int temp = Ready_Queue->ID[0];
				Ready_Queue->ID[0] = Currunt_CPU_Process;
				Currunt_CPU_Process = temp;
			}
		}

		CPU_Time++;

		if (Currunt_CPU_Process == -1 && Ready_Queue->cnt == 0) CPU_Idle_Time++;
		else {
			if (Currunt_CPU_Process == -1) {
				if (CPU_Idle_Time != 0) {
					CPU_Info[0][flag] = CPU_Time - 1;
					CPU_Info[1][flag] = -1;
					flag++;
					CPU_Idle_Time = 0;
				}
				Currunt_CPU_Process = Ready_Queue->ID[0];
				Dequeue(Ready_Queue);
			}
			process[Currunt_CPU_Process - 1].CPU_Burst_Time--;
			if (process[Currunt_CPU_Process - 1].CPU_Burst_Time == 0) {
				CPU_Info[0][flag] = CPU_Time;
				CPU_Info[1][flag] = Currunt_CPU_Process;
				flag++;
				if (process[Currunt_CPU_Process - 1].CPU_Burst_Time == process[Currunt_CPU_Process - 1].IO_Burst_Timing) {
					left_IO[4][Currunt_CPU_Process - 1] = -CPU_Time;
					Enqueue(Waiting_Queue, Currunt_CPU_Process);
				}
				Currunt_CPU_Process = -1;
				Process_End_Cnt++;
			}
			if (process[Currunt_CPU_Process - 1].CPU_Burst_Time == process[Currunt_CPU_Process - 1].IO_Burst_Timing) {
				left_IO[4][Currunt_CPU_Process - 1] = -CPU_Time;
				Enqueue(Waiting_Queue, Currunt_CPU_Process);
				CPU_Info[0][flag] = CPU_Time;
				CPU_Info[1][flag] = Currunt_CPU_Process;
				flag++;
				Currunt_CPU_Process = -1;
			}
		}

		if (Currunt_IO_Process != -1) process[Currunt_IO_Process - 1].IO_Burst_Time--;
		if (Currunt_IO_Process != -1 && process[Currunt_IO_Process - 1].IO_Burst_Time == 0) {
			if (process[Currunt_IO_Process - 1].CPU_Burst_Time > 0) {
				Enqueue(Ready_Queue, Currunt_IO_Process);
				for (int i = 0; i < Ready_Queue->cnt; i++) {
					for (int j = i + 1; j < Ready_Queue->cnt; j++) {
						if (process[Ready_Queue->ID[i] - 1].CPU_Burst_Time > process[Ready_Queue->ID[j] - 1].CPU_Burst_Time) {
							int temp = Ready_Queue->ID[i];
							Ready_Queue->ID[i] = Ready_Queue->ID[j];
							Ready_Queue->ID[j] = temp;
						}
					}
				}
				if (Currunt_CPU_Process != -1 && process[Currunt_CPU_Process - 1].CPU_Burst_Time > process[Ready_Queue->ID[0] - 1].CPU_Burst_Time) {
					CPU_Info[0][flag] = CPU_Time;
					CPU_Info[1][flag] = Currunt_CPU_Process;
					flag++;
					int temp = Ready_Queue->ID[0];
					Ready_Queue->ID[0] = Currunt_CPU_Process;
					Currunt_CPU_Process = temp;
				}
			}
			Currunt_IO_Process = -1;
		}
		if (Currunt_IO_Process == -1 && Waiting_Queue->cnt > 0) {
			Currunt_IO_Process = Waiting_Queue->ID[0];
			left_IO[4][Currunt_IO_Process - 1] += CPU_Time;
			Dequeue(Waiting_Queue);
		}

	}

	printf("P_SJF:\n");
	Print_Gantt_Chart(CPU_Info, flag);

	float P_SJf_Average_Waiting_Time = Calculate_Average_Waiting_Time(Process_Info, Process_Cnt, CPU_Info, flag, 4);
	float P_SJF_Average_Turnaround_Time = Calculate_Average_Turnaround_Time(Process_Info, Process_Cnt, CPU_Info, flag);
	float P_SJF_CPU_Utilization = Calculate_CPU_Utilization(CPU_Info, flag);

	Average_Waiting_Time[4] = P_SJf_Average_Waiting_Time;
	Average_Turnaround_Time[4] = P_SJF_Average_Turnaround_Time;
	CPU_Utilization[4] = P_SJF_CPU_Utilization;

	/*
	free(Ready_Queue->ID);
	free(Waiting_Queue->ID);
	free(Ready_Queue);
	free(Waiting_Queue);
	free(process);
	for (int i = 0; i < 2; i++) free(CPU_Info[i]);
	free(CPU_Info);
	*/
}

void P_Priority(Process Process_Info[], int Process_Cnt) {
	QUEUE* Ready_Queue = Create_Queue();
	QUEUE* Waiting_Queue = Create_Queue();

	Process* process = (Process*)malloc(sizeof(Process) * Process_Cnt);
	for (int i = 0; i < Process_Cnt; i++) process[i] = Copy_Process(Process_Info[i]);

	int Process_End_Cnt = 0;
	int flag = 0;

	int** CPU_Info = (int**)malloc(sizeof(int) * 2);
	for (int i = 0; i < 2; i++) CPU_Info[i] = (int*)malloc(sizeof(int) * 500);

	int Currunt_CPU_Process = -1;
	int Currunt_IO_Process = -1;
	int CPU_Idle_Time = 0;
	int CPU_Time = 0;

	while (Process_End_Cnt < Process_Cnt) {
		for (int i = 0; i < Process_Cnt; i++) if (CPU_Time == process[i].Arrival_Time) {
			Enqueue(Ready_Queue, i + 1);
			for (int j = 0; j < Ready_Queue->cnt; j++) {
				for (int k = j + 1; k < Ready_Queue->cnt; k++) {
					if (process[Ready_Queue->ID[j] - 1].Priority < process[Ready_Queue->ID[k] - 1].Priority) {
						int temp = Ready_Queue->ID[j];
						Ready_Queue->ID[j] = Ready_Queue->ID[k];
						Ready_Queue->ID[k] = temp;
					}
				}
			}
		}
		if (Currunt_CPU_Process != -1 && process[Currunt_CPU_Process - 1].Priority < process[Ready_Queue->ID[0] - 1].Priority) {
			CPU_Info[0][flag] = CPU_Time;
			CPU_Info[1][flag] = Currunt_CPU_Process;
			flag++;
			int temp = Ready_Queue->ID[0];
			Ready_Queue->ID[0] = Currunt_CPU_Process;
			Currunt_CPU_Process = temp;
		}

		CPU_Time++;

		if (Currunt_CPU_Process == -1 && Ready_Queue->cnt == 0) CPU_Idle_Time++;
		else {
			if (Currunt_CPU_Process == -1) {
				if (CPU_Idle_Time != 0) {
					CPU_Info[0][flag] = CPU_Time - 1;
					CPU_Info[1][flag] = -1;
					flag++;
					CPU_Idle_Time = 0;
				}
				Currunt_CPU_Process = Ready_Queue->ID[0];
				Dequeue(Ready_Queue);
			}
			process[Currunt_CPU_Process - 1].CPU_Burst_Time--;
			if (process[Currunt_CPU_Process - 1].CPU_Burst_Time == 0) {
				CPU_Info[0][flag] = CPU_Time;
				CPU_Info[1][flag] = Currunt_CPU_Process;
				flag++;
				if (process[Currunt_CPU_Process - 1].CPU_Burst_Time == process[Currunt_CPU_Process - 1].IO_Burst_Timing) {
					left_IO[5][Currunt_CPU_Process - 1] = -CPU_Time;
					Enqueue(Waiting_Queue, Currunt_CPU_Process);
				}
				Currunt_CPU_Process = -1;
				Process_End_Cnt++;
			}
			if (process[Currunt_CPU_Process - 1].CPU_Burst_Time == process[Currunt_CPU_Process - 1].IO_Burst_Timing) {
				left_IO[5][Currunt_CPU_Process - 1] = -CPU_Time;
				Enqueue(Waiting_Queue, Currunt_CPU_Process);
				CPU_Info[0][flag] = CPU_Time;
				CPU_Info[1][flag] = Currunt_CPU_Process;
				flag++;
				Currunt_CPU_Process = -1;
			}
		}

		if (Currunt_IO_Process != -1) process[Currunt_IO_Process - 1].IO_Burst_Time--;
		if (Currunt_IO_Process != -1 && process[Currunt_IO_Process - 1].IO_Burst_Time == 0) {
			if (process[Currunt_IO_Process - 1].CPU_Burst_Time > 0) {
				Enqueue(Ready_Queue, Currunt_IO_Process);
				for (int i = 0; i < Ready_Queue->cnt; i++) {
					for (int j = i + 1; j < Ready_Queue->cnt; j++) {
						if (process[Ready_Queue->ID[i] - 1].Priority < process[Ready_Queue->ID[j] - 1].Priority) {
							int temp = Ready_Queue->ID[i];
							Ready_Queue->ID[i] = Ready_Queue->ID[j];
							Ready_Queue->ID[j] = temp;
						}
					}
				}
				if (Currunt_CPU_Process != -1 && process[Currunt_CPU_Process - 1].Priority > process[Ready_Queue->ID[0] - 1].Priority) {
					CPU_Info[0][flag] = CPU_Time;
					CPU_Info[1][flag] = Currunt_CPU_Process;
					flag++;
					int temp = Ready_Queue->ID[0];
					Ready_Queue->ID[0] = Currunt_CPU_Process;
					Currunt_CPU_Process = temp;
				}
			}
			Currunt_IO_Process = -1;
		}
		if (Currunt_IO_Process == -1 && Waiting_Queue->cnt > 0) {
			Currunt_IO_Process = Waiting_Queue->ID[0];
			left_IO[5][Currunt_IO_Process - 1] += CPU_Time;
			Dequeue(Waiting_Queue);
		}
	}

	printf("P_Priority:\n");
	Print_Gantt_Chart(CPU_Info, flag);

	float P_Priority_Average_Waiting_Time = Calculate_Average_Waiting_Time(Process_Info, Process_Cnt, CPU_Info, flag, 5);
	float P_Priority_Average_Turnaround_Time = Calculate_Average_Turnaround_Time(Process_Info, Process_Cnt, CPU_Info, flag);
	float P_Priority_CPU_Utilization = Calculate_CPU_Utilization(CPU_Info, flag);

	Average_Waiting_Time[5] = P_Priority_Average_Waiting_Time;
	Average_Turnaround_Time[5] = P_Priority_Average_Turnaround_Time;
	CPU_Utilization[5] = P_Priority_CPU_Utilization;

	/*
	free(Ready_Queue->ID);
	free(Waiting_Queue->ID);
	free(Ready_Queue);
	free(Waiting_Queue);
	free(process);
	for (int i = 0; i < 2; i++) free(CPU_Info[i]);
	free(CPU_Info);
	*/
}

void Schedule(Process Process_Info[], int Process_Cnt, int quantum) {
	FCFS(Process_Info, Process_Cnt);
	SJF(Process_Info, Process_Cnt);
	Priority(Process_Info, Process_Cnt);
	RR(Process_Info, Process_Cnt, quantum);
	P_SJF(Process_Info, Process_Cnt);
	P_Priority(Process_Info, Process_Cnt);
}

void Evaluation() {
	float temp;
	char Rank_Name[6][11];
	strcpy(Rank_Name[0], "FCFS");
	strcpy(Rank_Name[1], "SJF");
	strcpy(Rank_Name[2], "Priority");
	strcpy(Rank_Name[3], "RR");
	strcpy(Rank_Name[4], "P_SJF");
	strcpy(Rank_Name[5], "P_Priority");

	for (int i = 0; i < 6; i++) {
		for (int j = i + 1; j < 6; j++) {
			if (Average_Waiting_Time[i] > Average_Waiting_Time[j]) {
				temp = Average_Waiting_Time[i];
				Average_Waiting_Time[i] = Average_Waiting_Time[j];
				Average_Waiting_Time[j] = temp;

				temp = Rank_Waiting_Time[i];
				Rank_Waiting_Time[i] = Rank_Waiting_Time[j];
				Rank_Waiting_Time[j] = temp;
			}
			if (Average_Turnaround_Time[i] > Average_Turnaround_Time[j]) {
				temp = Average_Turnaround_Time[i];
				Average_Turnaround_Time[i] = Average_Turnaround_Time[j];
				Average_Turnaround_Time[j] = temp;

				temp = Rank_Turnaround_Time[i];
				Rank_Turnaround_Time[i] = Rank_Turnaround_Time[j];
				Rank_Turnaround_Time[j] = temp;
			}
			
			if (CPU_Utilization[i] < CPU_Utilization[j]) {
				temp = CPU_Utilization[i];
				CPU_Utilization[i] = CPU_Utilization[j];
				CPU_Utilization[j] = temp;

				temp = Rank_CPU_Utilization[i];
				Rank_CPU_Utilization[i] = Rank_CPU_Utilization[j];
				Rank_CPU_Utilization[j] = temp;
			}
			
		}
	}

	printf("\nRank of Average Waiting Time: \n");
	for (int i = 0; i < 6; i++) {
		printf("%d. %s : %.1f\n", i + 1, Rank_Name[Rank_Waiting_Time[i]], Average_Waiting_Time[i]);
	}
	printf("\n");

	printf("\nRank of Average Turnaround Time: \n");
	for (int i = 0; i < 6; i++) {
		printf("%d. %s : %.1f\n", i + 1, Rank_Name[Rank_Turnaround_Time[i]], Average_Turnaround_Time[i]);
	}
	printf("\n");

	
	printf("\nRank of CPU Utilization: \n");
	for (int i = 0; i < 6; i++) {
		printf("%d. %s : %.0f%%\n", i + 1, Rank_Name[Rank_CPU_Utilization[i]], CPU_Utilization[i] * 100);
	}
	printf("\n");
	
}

int main(void) {
	int Process_Cnt = 5;
	int quantum = 4;

	srand(time(NULL));

	Process Process_Info[5];
	for (int i = 0; i < Process_Cnt; i++) {
		Process_Info[i] = Create_Process(i + 1);
	}

	Print_Process(Process_Info, Process_Cnt);

	Schedule(Process_Info, Process_Cnt, quantum);

	Evaluation();

	return 0;
}