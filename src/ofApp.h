#pragma once

#include "ofMain.h"
#include "ofxWinMenu.h"
#include <math.h>

#define MAX_DOTS 15 // 최대 점 개수 지정
#define DOT_RADIUS 10 // 점의 반지름 지정
#define LINE_WIDTH 5 // 직선의 두께를 지정
#define PATH_WIDTH 8 // 경로 직선의 두께를 지정
#define INF 9999999 // 무한대를 나타내는 숫자(경로 길이를 다루는데 사용)

struct Dot // 점의 정보 저장
{
	int x, y; // 점의 x, y좌표
	int idx; // 점이 그려진 순서 저장
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

		void addDot(); // 점을 추가하는 함수
		void selectDot(); // 직선 추가, 경로 탐색을 위해 점을 선택하는 함수
		void addLine(); // 직선을 추가하는 함수
		void calculatePath(); // 선택된 점 사이에 최적의 경로를 탐색하는 함수, Dijkstra's algorithm 사용
		void drawDot(); // 점을 그리는 함수
		void drawLine(); // 직선을 그리는 함수
		void drawLineLength(); // 직선의 길이를 그리는 함수
		void drawPath(); // 탐색한 경로를 그리는 함수
		
		// 화면 설정 관련
		HWND hWnd; // 프로그램의 창을 다루는 변수
		ofxWinMenu* menu; // 메뉴 객체를 저장
		void appMenuFunction(string title, bool bChecked); // 메뉴 함수

		// 기능 선택 여부 및 draw 관련
		int isDot, isLine, isPath; // 현재 선택된 기능이 어떤 것인지 저장
		int clickFlag; // 마우스 클릭이 입력되었는지 저장
		int isOnceHandled; // 점을 추가하거나 선택할 때 클릭 한 번당 추가/선택 과정이 한 번만 이뤄지게 하는 플래그

		// 점 정보 관련
		Dot dotArr[MAX_DOTS]; // 점의 정보를 저장하는 배열
		Dot selected[2]; // 현재 선택된 점의 정보를 저장하는 배열
		int dot_amount; // 점의 전체 개수
		int selected_amount; // 선택된 점의 개수

		// 직선 정보 관련
		int lineArr[MAX_DOTS][MAX_DOTS]; // 직선의 정보를 저장하는 배열

		// 경로 정보 관련
		int pathLen[MAX_DOTS]; // 경로 탐색의 시작점부터 각 점까지의 경로 거리를 저장하는 배열
		int parent[MAX_DOTS]; // 탐색한 경로를 다시 사용하기 위해, 경로에 포함된 각 점들의 이전 점을 저장하는 배열
		int found[MAX_DOTS]; // 경로를 탐색할 때, 각 점이 이미 탐색 경로에 포함되었는지(방문 여부)를 저장하는 배열
		int endidx; // 경로의 끝 점의 인덱스를 저장
};
