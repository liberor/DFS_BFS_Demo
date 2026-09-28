#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <easyx.h>
#include <windows.h>

#define SIZE 21 //length of a single squre's edge
				//(525 / SIZE) length&width of map
#define SLEEP 6

int isLBUTTONstillDown = 0;
int lastPosAltered[2] = { -1 , -1 };

enum method { DFS, BFS };

typedef int ElemType;

typedef struct QueueNode {
	ElemType data;	//index of vertex here
	struct QueueNode* prev;
	struct QueueNode* next;
} QueueNode;

typedef struct Queue {
	QueueNode* head;
	int length;
} Queue;

Queue* initQueueBLL() {
	Queue* q = (Queue*)malloc(sizeof(Queue));
	if (q == NULL) exit(0);
	q->length = 0;
	q->head = (QueueNode*)malloc(sizeof(QueueNode));
	if (q->head == NULL) exit(0);
	//head->data not initialized
	q->head->prev = NULL;
	q->head->next = NULL;
	return q;
}

int enQueue(Queue* q, ElemType e) {
	QueueNode* p = (QueueNode*)malloc(sizeof(QueueNode));
	if (p == NULL) return 0;
	p->data = e;
	QueueNode* tail = q->head;
	int length = q->length;
	while (length--) tail = tail->next;
	p->prev = tail;
	p->next = NULL;
	tail->next = p;
	q->length++;
	return 1;
}

int deQueue(Queue* q, ElemType* ep) {
	if (q->length == 0) return 0;
	QueueNode* head = q->head;
	QueueNode* first = head->next;
	head->next = NULL;
	if (q->length >= 2) {
		QueueNode* second = first->next;
		head->next = second;
		second->prev = head;
	}
	*ep = first->data;
	free(first);
	q->length--;
	return 1;
}
void freeQueue(Queue* q) {
	ElemType garbage;
	for (int i = 0; i < q->length; i++) {
		deQueue(q, &garbage);
	}
	free(q->head);
	free(q);
	return;
}


void drawMap(char* map, int length, int width, int* start_pos, int* fin_pos);
void drawText(int start, int finish);
int countRoad(char* map, int length, int width);
void initVertex(char** vertex, char* map, int length, int width);
void initMatrix(char* array, char** vertex, int total, char* map, int length, int width);
void findStartAndFinishIndexinVertex(char** vertex, int total, char* map, int length, int width, int* start_index, int* finish_index, int start, int finish);
void showResult(char* result, int length, int width, int* start_pos, int* fin_pos);
void dfs(int total, char* array, char* visited, char** vertex, char* map, int length, int width, char* final_path, int start_index, int finish_index, int real_start_index);
void bfs(int total, char* array, char* visited, char** vertex, char* map, int length, int width, char* final_path, int start_index, int finish_index);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	enum method flag = DFS;

	int length = 525 / SIZE;
	int width = 525 / SIZE;
	//initialize the map
	char* map = (char*)malloc(sizeof(char) * length * width);
	if(map == NULL) exit(0);
	memset(map, 1, length * width);

	//input start and finish position
	int start_pos[2] = { -1,-1 };
	int start = start_pos[0] * width + start_pos[1];

	int fin_pos[2] = { -1,-1 };
	int finish = fin_pos[0] * width + fin_pos[1];

	initgraph(800, 525);
	setbkcolor(RGB(233, 233, 233));
	settextcolor(RGB(148, 0, 211));
	settextstyle(21, 0, "Consolas");

	BeginBatchDraw();

	cleardevice();
	drawMap(map, length, width, start_pos, fin_pos);
	outtextxy(600, 20, "road");
	outtextxy(600, 45, "wall");
	outtextxy(600, 70, "start");
	outtextxy(600, 95, "destination");
	outtextxy(600, 125, "visited road");
	outtextxy(600, 150, "path found");
	setfillcolor(LIGHTGRAY);
	solidrectangle(565, 23, 565 + SIZE -1, 23 + SIZE - 1);
	setfillcolor(BLACK);
	solidrectangle(565, 48, 565 + SIZE - 1, 48 + SIZE - 1);
	setfillcolor(LIGHTRED);
	solidrectangle(565, 73, 565 + SIZE - 1, 73 + SIZE - 1);
	setfillcolor(LIGHTBLUE);
	solidrectangle(565, 98, 565 + SIZE - 1, 98 + SIZE - 1);
	setfillcolor(YELLOW);
	solidrectangle(565, 128, 565 + SIZE - 1, 128 + SIZE - 1);
	setfillcolor(RED);
	solidrectangle(565, 153, 565 + SIZE - 1, 153 + SIZE - 1);
	settextstyle(25, 0, "Consolas");
	settextcolor(RED);
	outtextxy(558, 300, "Depth-First Search");
	outtextxy(543, 380, "Breadth-First Search");
	rectangle(548, 300, 548 + 233, 300 + 30);
	rectangle(540, 380, 540 + 248, 380 + 30);
	EndBatchDraw();

	ExMessage msg = { 0 };
loop:
	while (peekmessage(&msg ));
	while (1) {
		if (peekmessage(&msg, EX_MOUSE)) {
			if ((msg.message == WM_MOUSEMOVE && isLBUTTONstillDown == 1)) {
				//alter map
				if (msg.x < SIZE * width) {
					int i_index = msg.y / SIZE;
					int j_index = msg.x / SIZE;
					if (i_index == lastPosAltered[0] && j_index == lastPosAltered[1]) {
						continue;
					}
					lastPosAltered[0] = i_index;
					lastPosAltered[1] = j_index;
					//type of pos clicked
					if (map[i_index * width + j_index] == 1) {
						//start
						if (i_index == start_pos[0] && j_index == start_pos[1]) {
							start_pos[0] = -1;
							start_pos[1] = -1;
							start = start_pos[0] * width + start_pos[1];
						}
						//dest
						else if (i_index == fin_pos[0] && j_index == fin_pos[1]) {
							fin_pos[0] = -1;
							fin_pos[1] = -1;
							finish = fin_pos[0] * width + fin_pos[1];
						}
						//road
						else {
							//no start set
							if (start_pos[0] == -1) {
								start_pos[0] = i_index;
								start_pos[1] = j_index;
								start = start_pos[0] * width + start_pos[1];
							}
							//no dest set
							else if (fin_pos[0] == -1) {
								fin_pos[0] = i_index;
								fin_pos[1] = j_index;
								finish = fin_pos[0] * width + fin_pos[1];
							}
							else {
								map[i_index * width + j_index] = 0;
							}
						}
					}
					//wall
					else {
						map[i_index * width + j_index] = 1;
					}
					BeginBatchDraw();
					if (map[i_index * width + j_index] == 1) {
						setfillcolor(LIGHTGRAY);
						solidrectangle(j_index * SIZE, i_index * SIZE, j_index * SIZE + SIZE - 1, i_index * SIZE + SIZE - 1);
					}
					else {
						setfillcolor(BLACK);
						solidrectangle(j_index * SIZE, i_index * SIZE, j_index * SIZE + SIZE - 1, i_index * SIZE + SIZE - 1);
					}
					if (start_pos[0] != -1) {
						setfillcolor(LIGHTRED);
						solidrectangle(start_pos[1] * SIZE, start_pos[0] * SIZE, start_pos[1] * SIZE + SIZE - 1, start_pos[0] * SIZE + SIZE - 1);
					}
					if (fin_pos[0] != -1) {
						setfillcolor(LIGHTBLUE);
						solidrectangle(fin_pos[1] * SIZE, fin_pos[0] * SIZE, fin_pos[1] * SIZE + SIZE - 1, fin_pos[0] * SIZE + SIZE - 1);
					}
					drawText(start, finish);
					EndBatchDraw();
				}
			}
			else if (msg.message == WM_LBUTTONDOWN) {
				isLBUTTONstillDown = 1;
				BeginBatchDraw();
				drawMap(map, length, width, start_pos, fin_pos);
				EndBatchDraw();
				//alter map
				if (msg.x < SIZE * width) {
					int i_index = msg.y / SIZE;
					int j_index = msg.x / SIZE;
					lastPosAltered[0] = i_index;
					lastPosAltered[1] = j_index;
					//type of pos clicked
					if (map[i_index * width + j_index] == 1) {
						//start
						if (i_index == start_pos[0] && j_index == start_pos[1]) {
							start_pos[0] = -1;
							start_pos[1] = -1;
							start = start_pos[0] * width + start_pos[1];
						}
						//dest
						else if (i_index == fin_pos[0] && j_index == fin_pos[1]) {
							fin_pos[0] = -1;
							fin_pos[1] = -1;
							finish = fin_pos[0] * width + fin_pos[1];
						}
						//road
						else {
							//no start set
							if (start_pos[0] == -1) {
								start_pos[0] = i_index;
								start_pos[1] = j_index;
								start = start_pos[0] * width + start_pos[1];
							}
							//no dest set
							else if (fin_pos[0] == -1) {
								fin_pos[0] = i_index;
								fin_pos[1] = j_index;
								finish = fin_pos[0] * width + fin_pos[1];
							}
							else {
								map[i_index * width + j_index] = 0;
							}
						}
					}
					//wall
					else {
						map[i_index * width + j_index] = 1;
					}
					BeginBatchDraw();
					if (map[i_index * width + j_index] == 1) {
						setfillcolor(LIGHTGRAY);
						solidrectangle(j_index * SIZE, i_index * SIZE, j_index * SIZE + SIZE - 1, i_index * SIZE + SIZE - 1);
					}
					else {
						setfillcolor(BLACK);
						solidrectangle(j_index * SIZE, i_index * SIZE, j_index * SIZE + SIZE - 1, i_index * SIZE + SIZE - 1);
					}
					if (start_pos[0] != -1) {
						setfillcolor(LIGHTRED);
						solidrectangle(start_pos[1] * SIZE, start_pos[0] * SIZE, start_pos[1] * SIZE + SIZE - 1, start_pos[0] * SIZE + SIZE - 1);
					}
					if (fin_pos[0] != -1) {
						setfillcolor(LIGHTBLUE);
						solidrectangle(fin_pos[1] * SIZE, fin_pos[0] * SIZE, fin_pos[1] * SIZE + SIZE - 1, fin_pos[0] * SIZE + SIZE - 1);
					}
					drawText(start, finish);
					EndBatchDraw();
				}
				//dfs
				else if (msg.x >= 548 && msg.x <= 548 + 233 && msg.y >= 300 && msg.y <= 300 + 30) {
					if (start >= 0 && finish >= 0) {
						flag = DFS;
						goto search;
					}
				}
				//bfs
				else if (msg.x >= 540 && msg.x <= 540 + 248 && msg.y >= 380 && msg.y <= 380 + 30) {
					if (start >= 0 && finish >= 0) {
						flag = BFS;
						goto search;
					}
				}
			}
			else if (msg.message == WM_LBUTTONUP) {
				isLBUTTONstillDown = 0;
			}
		}
	}
search:
	isLBUTTONstillDown = 0;
	int total = countRoad(map, length, width);
	char** vertex = (char**)malloc(total * sizeof(char*));
	initVertex(vertex, map, length, width);
	char* array = (char*)malloc(sizeof(char) * total * total);
	initMatrix(array, vertex, total, map, length, width);

	int start_index = 0;
	int finish_index = 0;
	findStartAndFinishIndexinVertex(vertex, total, map, length, width, &start_index, &finish_index, start, finish);

	char* visited = (char*)malloc(sizeof(char) * total);
	for (int i = 0; i < total; i++) visited[i] = 0;
	char* final_path = (char*)malloc(sizeof(char) * total);
	for (int i = 0; i < total; i++) final_path[i] = 0;

	char* result = (char*)malloc(sizeof(char) * length * width);
	memcpy(result, map, length * width);

	BeginBatchDraw();
	drawMap(map, length, width, start_pos, fin_pos);
	EndBatchDraw();

	if (flag == DFS) {
		dfs(total, array, visited, vertex,  map,  length,  width,final_path, start_index, finish_index, start_index);
		for (int i = 0; i < total; i++) {
			if (visited[i] == 1) {
				result[vertex[i] - map] = 3;
			}
			visited[i] = 0;
		}
		for (int i = 0; i < total; i++) {
			if (final_path[i] == 1) {
				result[vertex[i] - map] = 2;
			}
			final_path[i] = 0;
		}
		showResult(result, length, width, start_pos, fin_pos);
	}

	else {
		bfs(total, array, visited, vertex, map, length, width, final_path, start_index, finish_index);
		for (int i = 0; i < total; i++) {
			if (visited[i] == 1) {
				result[vertex[i] - map] = 3;
			}
			visited[i] = 0;
		}
		for (int i = 0; i < total; i++) {
			if (final_path[i] == 1) {
				result[vertex[i] - map] = 2;
			}
			final_path[i] = 0;
		}
		showResult(result, length, width, start_pos, fin_pos);
	}
	goto loop;
	return 0;
}
int countRoad(char* map, int length, int width) {
	int count_road = 0;
	for (int i = 0; i < length; i++) {
		for (int j = 0; j < width; j++) {
			if (map[i * width + j] == 1) count_road++;
		}
	}
	return count_road;
}
void initVertex(char** vertex, char* map, int length, int width) {
	int idx = 0;
	for (int i = 0; i < length; i++) {
		for (int j = 0; j < width; j++) {
			if (map[i * width + j] == 1) vertex[idx++] = &map[i * width + j];
		}
	}
}
void initMatrix(char* array, char** vertex, int total, char* map, int length, int width) {
	for (int i = 0; i < total; i++) {
		for (int j = 0; j < total; j++) {
			if (i == j) {
				array[i * total + j] = 0;
			}
			else {
				if (vertex[i] == vertex[j] - 1 && (vertex[i] - map + 1) % width != 0) {
					array[i * total + j] = 1;
				}
				else if (vertex[i] == vertex[j] + 1 && (vertex[i] - map) % width != 0) {
					array[i * total + j] = 1;
				}
				else if (vertex[i] == vertex[j] + width && vertex[i] - map >= width) {
					array[i * total + j] = 1;
				}
				else if (vertex[i] == vertex[j] - width && vertex[i] - map <= length * width - width - 1) {
					array[i * total + j] = 1;
				}
				else {
					array[i * total + j] = 0;
				}
			}
		}
	}
}

void findStartAndFinishIndexinVertex(char** vertex, int total, char* map, int length, int width, int* start_index, int* finish_index, int start, int finish) {
	char* start_p = &map[(start / width) * width + (start % width)];
	char* fin_p = &map[(finish / width) * width + (finish % width)];
	for (int i = 0; i < total; i++) {
		if (vertex[i] == start_p) *start_index = i;
		if (vertex[i] == fin_p) *finish_index = i;
	}
}
void showResult(char* result, int length, int width, int* start_pos, int* fin_pos) {
	for (int i = 0; i < length; i++) {
		for (int j = 0; j < width; j++) {
			//road not visited
			if (result[i * width + j] == 1) {
				setfillcolor(LIGHTGRAY);
				solidrectangle(j * SIZE, i * SIZE, j * SIZE + SIZE - 1, i * SIZE + SIZE - 1);
			}
			//wall
			else if (result[i * width + j] == 0) {
				setfillcolor(BLACK);
				solidrectangle(j * SIZE, i * SIZE, j * SIZE + SIZE - 1, i * SIZE + SIZE - 1);
			}
			//road visited
			else if (result[i * width + j] == 3) {
				setfillcolor(YELLOW);
				solidrectangle(j * SIZE, i * SIZE, j * SIZE + SIZE - 1, i * SIZE + SIZE - 1);
			}
			//path found
			else {
				setfillcolor(RED);
				solidrectangle(j * SIZE, i * SIZE, j * SIZE + SIZE - 1, i * SIZE + SIZE - 1);
			}
		}
	}
	if (start_pos[0] != -1) {
		setfillcolor(LIGHTRED);
		solidrectangle(start_pos[1] * SIZE, start_pos[0] * SIZE, start_pos[1] * SIZE + SIZE - 1, start_pos[0] * SIZE + SIZE - 1);
	}
	if (fin_pos[0] != -1) {
		setfillcolor(LIGHTBLUE);
		solidrectangle(fin_pos[1] * SIZE, fin_pos[0] * SIZE, fin_pos[1] * SIZE + SIZE - 1, fin_pos[0] * SIZE + SIZE - 1);
	}
}

void dfs(int total, char* array, char* visited,char** vertex,char* map,int length,int width, char* final_path, int start_index, int finish_index,int real_start_index) {
	visited[start_index] = 1;
	if(start_index != real_start_index){
		int i_idx = (vertex[start_index] - map) / width;
		int j_idx = (vertex[start_index] - map) % width;
		setfillcolor(YELLOW);
		BeginBatchDraw();
		solidrectangle(j_idx * SIZE, i_idx * SIZE, j_idx * SIZE + SIZE - 1, i_idx * SIZE + SIZE - 1);
		EndBatchDraw();
		Sleep(SLEEP);
	}
	if (start_index == finish_index) {
		final_path[finish_index] = 1;
		return;
	}
	int i = start_index;
	for (int j = 0; j < total; j++) {
		if (array[i * total + j] == 1 && visited[j] == 0) {
			//printf("%d %d\n",i,j); search path
			dfs(total, array, visited,  vertex,  map, length, width,final_path, j, finish_index, real_start_index);
		}
		if (visited[finish_index] == 1) {
			//printf("%d %d\n", i, j);
			final_path[i] = 1;
			break;
		}
	}
}

void bfs(int total, char* array, char* visited, char** vertex, char* map, int length,int width , char* final_path, int start_index, int finish_index) {
	Queue* q = initQueueBLL();
	int curr;
	int count = 0;
	int* parent = (int*)malloc(sizeof(int) * total);
	for (int i = 0; i < total; i++) parent[i] = -1;
	enQueue(q, start_index);
	count++;
	visited[start_index] = 1;
	while (count >= 1)
	{
		int rare_count = count;
		for (int i = 0; i < rare_count; i++) {
			deQueue(q, &curr);
			count--;
			for (int j = 0; j < total; j++) {
				if (array[curr * total + j] == 1 && visited[j] == 0) {
					enQueue(q, j);
					count++;
					visited[j] = 1;
					int i_idx = (vertex[j] - map) / width;
					int j_idx = (vertex[j] - map) % width;
					setfillcolor(YELLOW);
					BeginBatchDraw();
					solidrectangle(j_idx * SIZE, i_idx * SIZE, j_idx * SIZE + SIZE - 1, i_idx * SIZE + SIZE - 1);
					EndBatchDraw();
					Sleep(SLEEP);
					parent[j] = curr;
					//printf("%d %d\n", curr, j);
					if (visited[finish_index] == 1) {
						//printf("%d %d\n", curr, j);
						break;
					}
				}
			}
			if (visited[finish_index] == 1) {
				break;
			}
		}
		if (visited[finish_index] == 1) {
			break;
		}
	}
	if (visited[finish_index] == 1) {
		final_path[finish_index] = 1;
		int parent_index = parent[finish_index];
		while (parent_index != -1)
		{
			final_path[parent_index] = 1;
			parent_index = parent[parent_index];
		}
	}
	freeQueue(q);
}
void drawMap(char* map, int length, int width, int* start_pos, int* fin_pos) {
	for (int i = 0; i < length; i++) {
		for (int j = 0; j < width; j++) {
			if (map[i * width + j] == 1) {
				setfillcolor(LIGHTGRAY);
				solidrectangle(j * SIZE, i * SIZE, j * SIZE + SIZE - 1, i * SIZE + SIZE - 1);
			}
			else {
				setfillcolor(BLACK);
				solidrectangle(j * SIZE, i * SIZE, j * SIZE + SIZE - 1, i * SIZE + SIZE - 1);
			}
		}
	}
	if (start_pos[0] != -1) {
		setfillcolor(LIGHTRED);
		solidrectangle(start_pos[1] * SIZE, start_pos[0] * SIZE, start_pos[1] * SIZE + SIZE - 1, start_pos[0] * SIZE + SIZE - 1);
	}
	if (fin_pos[0] != -1) {
		setfillcolor(LIGHTBLUE);
		solidrectangle(fin_pos[1] * SIZE, fin_pos[0] * SIZE, fin_pos[1] * SIZE + SIZE - 1, fin_pos[0] * SIZE + SIZE - 1);
	}
}
void drawText(int start, int finish) {
	if (start < 0 || finish < 0) settextcolor(RED);
	else settextcolor(GREEN);
	outtextxy(558, 300, "Depth-First Search");
	outtextxy(543, 380, "Breadth-First Search");
}