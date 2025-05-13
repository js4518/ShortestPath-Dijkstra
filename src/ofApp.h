#pragma once

#include "ofMain.h"
#include "ofxWinMenu.h"
#include <math.h>

#define MAX_DOTS 15 // �ִ� �� ���� ����
#define DOT_RADIUS 10 // ���� ������ ����
#define LINE_WIDTH 5 // ������ �β��� ����
#define PATH_WIDTH 8 // ��� ������ �β��� ����
#define INF 9999999 // ���Ѵ븦 ��Ÿ���� ����(��� ���̸� �ٷ�µ� ���)

struct Dot // ���� ���� ����
{
	int x, y; // ���� x, y��ǥ
	int idx; // ���� �׷��� ���� ����
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void addDot(); // ���� �߰��ϴ� �Լ�
		void selectDot(); // ���� �߰�, ��� Ž���� ���� ���� �����ϴ� �Լ�
		void addLine(); // ������ �߰��ϴ� �Լ�
		void calculatePath(); // ���õ� �� ���̿� ������ ��θ� Ž���ϴ� �Լ�, Dijkstra's algorithm ���
		void drawDot(); // ���� �׸��� �Լ�
		void drawLine(); // ������ �׸��� �Լ�
		void drawLineLength(); // ������ ���̸� �׸��� �Լ�
		void drawPath(); // Ž���� ��θ� �׸��� �Լ�
		
		// ȭ�� ���� ����
		HWND hWnd; // ���α׷��� â�� �ٷ�� ����
		ofxWinMenu* menu; // �޴� ��ü�� ����
		void appMenuFunction(string title, bool bChecked); // �޴� �Լ�

		// ��� ���� ���� �� draw ����
		int isDot, isLine, isPath; // ���� ���õ� ����� � ������ ����
		int clickFlag; // ���콺 Ŭ���� �ԷµǾ����� ����
		int isOnceHandled; // ���� �߰��ϰų� ������ �� Ŭ�� �� ���� �߰�/���� ������ �� ���� �̷����� �ϴ� �÷���

		// �� ���� ����
		Dot dotArr[MAX_DOTS]; // ���� ������ �����ϴ� �迭
		Dot selected[2]; // ���� ���õ� ���� ������ �����ϴ� �迭
		int dot_amount; // ���� ��ü ����
		int selected_amount; // ���õ� ���� ����

		// ���� ���� ����
		int lineArr[MAX_DOTS][MAX_DOTS]; // ������ ������ �����ϴ� �迭

		// ��� ���� ����
		int pathLen[MAX_DOTS]; // ��� Ž���� ���������� �� �������� ��� �Ÿ��� �����ϴ� �迭
		int parent[MAX_DOTS]; // Ž���� ��θ� �ٽ� ����ϱ� ����, ��ο� ���Ե� �� ������ ���� ���� �����ϴ� �迭
		int found[MAX_DOTS]; // ��θ� Ž���� ��, �� ���� �̹� Ž�� ��ο� ���ԵǾ�����(�湮 ����)�� �����ϴ� �迭
		int endidx; // ����� �� ���� �ε����� ����
};
