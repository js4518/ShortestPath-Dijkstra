#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	// 창 이름 설정
	ofSetWindowTitle("Shortest Path Finder");

	// 화면 색 설정
	ofBackground(ofColor::white);

	// 창 메뉴 추가
	hWnd = WindowFromDC(wglGetCurrentDC()); // 현재 다루는 창 불러오기
	menu = new ofxWinMenu(this, hWnd); // 메뉴 오브젝트에 새로운 메뉴를 할당
	menu->CreateMenuFunction(&ofApp::appMenuFunction); // 메뉴에 대해 작동할 함수 설정
	HMENU hMenu = menu->CreateWindowMenu(); // 실제 창에 메뉴 추가
	HMENU hPopup = menu->AddPopupMenu(hMenu, "기능"); // 메뉴에 "기능" 팝업 추가
	menu->AddPopupItem(hPopup, "점 그리기", false, false); // 팝업에 "점 그리기" 기능 추가
	menu->AddPopupItem(hPopup, "직선 그리기", false, false); // 팝업에 "직선 그리기" 기능 추가
	menu->AddPopupItem(hPopup, "최단 경로 찾기", false, false); // 팝업에 "최단 경로 찾기" 기능 추가
	menu->AddPopupItem(hPopup, "종료", false, false); // 팝업에 "종료" 기능 추가
	menu->SetWindowMenu(); // 앞에서 설정한대로 창의 메뉴 지정
	
	// 변수 초기화
	isDot = false;
	isLine = false;
	isPath = false;
	clickFlag = false;
	isOnceHandled = false;
	dot_amount = 0;
	selected_amount = 0;
	endidx = -1;

	Dot newDot; // selected 배열의 초기화를 위해 Dot 타입 변수를 새로 선언
	newDot.idx = -1;
	selected[0] = newDot; selected[1] = newDot; // selected 배열 초기화

	for (int i = 0; i < MAX_DOTS; i++) // 이중 반복문을 이용하여 lineArr 배열 초기화
		for (int j = 0; j < MAX_DOTS; j++)
		{
			if (i == j) lineArr[i][j] = 0; // 자기 자신으로의 직선 길이를 0으로 설정
			else lineArr[i][j] = INF; // 그렇지 않다면 무한대로 설정
		}
}

void ofApp::appMenuFunction(string title, bool bChecked) 
{
	if (title == "점 그리기")  // "점 그리기" 기능이 선택된 경우
	{
		// isDot을 true로 하여 선택되었다는 것을 저장
		// 또한 나머지 기능을 비활성화 하기 위해 isLine, isPath를 false로 한다
		isDot = true;
		isLine = false;
		isPath = false;
	}
	if (title == "직선 그리기") // "직선 그리기" 기능이 선택된 경우
	{
		// 위와 같음
		isDot = false;
		isLine = true;
		isPath = false;
	}
	if (title == "최단 경로 찾기") // "최단 경로 찾기" 기능이 선택된 경우
	{
		// 위와 같음
		isDot = false;
		isLine = false;
		isPath = true;

		// 반복문을 이용하여 parent 배열을 초기화(+ 이전에 계산한 경로를 지운다)
		for (int i = 0; i < dot_amount; i++) 
			parent[i] = i; // 각 점의 부모를 자기 자신으로 설정
		endidx = -1; // 경로의 끝 점 정보 초기화
	}
	if(title == "종료") ofExit(); // "종료" 기능이 선택된 경우 프로그램 종료
}


//--------------------------------------------------------------
void ofApp::update(){
	if (isDot) // "점 그리기" 기능이 선택된 경우
		addDot(); // 점을 추가하는 함수를 호출
	else if (isLine || isPath) // "직선 그리기" 또는 "최뎐 경로 찾기" 기능이 선택된 경우
		selectDot(); // 점을 선택하는 함수를 호출
}

//--------------------------------------------------------------
void ofApp::draw(){
	string pLen; // 경로의 길이를 문자열 형태로 저장하는 변수

	ofSetColor(ofColor::black); // 그림의 색을 검정으로 지정
	ofDrawBitmapString("Click -> Draw Dot / Select Dot", 30, 30); // 안내 문구 그리기

	if (isPath && endidx != -1) // "최단 경로 찾기" 기능이 선택되었고, 경로 탐색이 이루어졌을 경우
		if (pathLen[endidx] != INF) // 또한 경로가 올바르게 찾아졌을 경우(선택된 점 사이에 경로가 존재)
		{
			pLen = "Path Length : " + ofToString(pathLen[endidx]); // ofToString()을 이용하여 경로의 길이를 문자열로 바꾸고 pLen에 저장
			ofDrawBitmapString(pLen, 30, 60); // 경로의 길이 그리기
		}

	drawLine(); // 직선을 그리는 함수를 호출
	drawDot(); // 점을 그리는 함수를 호출
	if (isPath) drawPath(); // "최단 경로 찾기" 기능이 선택되었다면 경로를 그리는 함수 호출
	drawLineLength();// 직선의 길이를 그리는 함수 호출
}

void ofApp::addDot()
{
	int x, y; // 마우스의 위치를 저장하는 변수
	Dot newDot; // 배열에 새로운 점의 정보를 추가하기 위한 변수

	if (clickFlag && !isOnceHandled) // 클릭이 이루어졌고 아직 클릭 관련 처리가 이루어지지 않았을 시
	{
		if (dot_amount != MAX_DOTS) // 또한 점의 개수가 최대에 도달하여 더이상 점을 그리지 못하는 상황이 아닐 시
		{
			x = ofGetMouseX(); y = ofGetMouseY(); // 마우스 위치를 저장
			newDot.x = x; newDot.y = y; // 새로운 점에 대한 Dot 객체에 위치 저장
			newDot.idx = dot_amount; // 인덱스도 저장
			dotArr[dot_amount++] = newDot; // dotArr 배열에 점을 저장
		}
		else printf("점 개수가 최대입니다.\n"); // 안내 메세지 출력
		isOnceHandled = true; // 클릭 관련 처리가 이루어졌음을 저장
	}
}

void ofApp::selectDot()
{
	int x, y; // 마우스의 위치를 저장하는 변수
	int dis; // 마우스의 위치와 점 사이의 거리를 저장하는 변수
	Dot curDot; // 반복문에서 현재 점의 정보를 저장하는 변수

	if (clickFlag && !isOnceHandled) // 클릭이 이루어졌고 아직 클릭 관련 처리가 이루어지지 않았을 시
	{
		x = ofGetMouseX(), y = ofGetMouseY(); // 마우스 위치를 저장
		for (int i = 0; i < dot_amount; i++) // 반복문을 이용하여 현재 마우스의 위치가 각 점의 위에 있는지 판정
		{
			curDot = dotArr[i]; // 현재 점의 정보를 저장
			dis = (x - curDot.x) * (x - curDot.x) + (y - curDot.y) * (y - curDot.y); // 사이 거리 계산
			if (dis < DOT_RADIUS * DOT_RADIUS)
			{
				selected[selected_amount++] = curDot; // 마우스의 위치가 점의 위에 있으면 그 점을 selected 배열에 저장
				break; // 한 점에 대해서만 처리
			}
		}
		if (selected_amount == 2) // 선택된 점의 개수가 2개일 시
		{
			selected_amount = 0; // 선택된 점의 개수 초기화
			if (isLine) addLine(); // "직선 그리기" 기능이 선택된 상황이라면 선택된 두 점 사이에 직선을 그리는 함수 호출
			if (isPath) calculatePath(); // "최단 경로 찾기" 기능이 선택된 상황이라면 선택된 두 점 사이의 최단 경로를 탐색하는 함수 호출
		}
		isOnceHandled = true; // 클릭 관련 처리가 이루어졌음을 저장
	}
}

void ofApp::addLine()
{
	Dot d1 = selected[0], d2 = selected[1]; // 현재 선택된 점의 정보를 저장하는 변수
	Dot newDot; // selected 배열 초기화를 위한 변수
	newDot.idx = -1;
	selected[0] = newDot; selected[1] = newDot; // selected 배열 초기화

	if (d1.idx == d2.idx) return; // 동일한 점이 두 번 선택되었을 시 처리 X

	int dis = sqrt((d1.x - d2.x) * (d1.x - d2.x) + (d1.y - d2.y) * (d1.y - d2.y)); // 선택된 두 점 사이의 거리 계산
	lineArr[d1.idx][d2.idx] = dis; // lineArr 배열에 시작점에서 끝점 사이의 거리 저장
}

void ofApp::calculatePath()
{
	int d1 = selected[0].idx, d2 = selected[1].idx; // 현재 선택된 점의 정보를 저장하는 변수
	int curd; // 반복문에서, 아직 방문하지 않았고 시작점과 가장 가까이 있는 점(최단거리 점)의 인덱스를 저장하는 변수
	int minLen; // 가장 가까이 있는 점을 찾기 위해 현재 반복까지의 최단거리를 저장하는 변수

	Dot newDot; // selected 배열 초기화를 위한 변수
	newDot.idx = -1;
	selected[0] = newDot; selected[1] = newDot; // selected 배열 초기화

	if (d1 == d2) return; // 동일한 점이 두 번 선택되었을 시 처리 X

	for (int i = 0; i < dot_amount; i++) // 반복문을 이용하여 각 점의 정보 초기화
	{
		found[i] = false; // 현재 점의 방문 여부를 초기화
		pathLen[i] = lineArr[d1][i]; // 시작점에서 현재 점까지의 거리 저장 
		parent[i] = i; // 현재 점의 부모 정보를 초기화(자기 자신으로 설정)
		if (pathLen[i] < INF) parent[i] = d1; // 현재 점이 시작점과 이어져있다면 현재 점의 부모를 시작점으로 설정
	}
	found[d1] = true; // 시작점의 방문 여부 저장

	for (int i = 0; i < dot_amount - 2; i++) // 반복문을 이용하여 (점 개수-2)번만큼 처리를 반복하여 시작점에서 모든 점까지의 경로 거리 계산
	{
		minLen = INF; // 최단거리 초기화
		curd = -1; // 최단거리 점 초기화
		for (int j = 0; j < dot_amount; j++) // 반복문을 이용하여, 방문하지 않았고 시작점과 가장 가까이 있는 점(최단거리 점) 찾기
			if (pathLen[j] < minLen && !found[j]) // 시작점과 연결되어있지 않은 점은 거리 비교에서 탈락
			{
				minLen = pathLen[j];
				curd = j;
			}
		
		if (curd == -1) break; // 시작점과 연결되어있는 점중 방문하지 않은 점이 없을시 종료

		found[curd] = true; // 최단거리 점의 방문 여부 저장 
		for (int j = 0; j < dot_amount; j++) // 반복문을 이용하여, 최단거리 점과 아직 방문하지 않은 점 사이에 대한 처리
			if (!found[j]) // 방문 여부 판단
				if (pathLen[curd] + lineArr[curd][j] < pathLen[j]) // "시작점에서 최단거리 점까지의 경로 거리 + 최단거리 점에서 현재 점까지의 거리 < 시작점에서 현재 점까지의 경로 거리"일시
				{
					pathLen[j] = pathLen[curd] + lineArr[curd][j]; // 시작점에서 현재 점까지의 경로 거리 업데이트
					parent[j] = curd; // 현재 점의 부모를 최단거리 점으로 저장
				}
	}

	endidx = d2; // 끝점의 인덱스 저장
}

void ofApp::drawDot()
{
	Dot curDot; // 반복문에서 현재 점의 정보를 저장하는 변수
	string dotNum; // 반복문에서 현재 점의 인덱스 숫자를 문자열로 저장하기 위한 변수

	for (int i = 0; i < dot_amount; i++) // 반복문을 이용하여 dotArr 배열에 저장된 모든 점을 그리기
	{
		curDot = dotArr[i]; // curDot에 현재 점의 정보 저장
		dotNum = ofToString(i); // ofToString()을 이용하여 현재 점의 인덱스를 문자열로 저장
		if (curDot.idx == selected[0].idx || curDot.idx == selected[1].idx) ofSetColor(ofColor::red); // 현재 점이 선택된 점이면 그림의 색을 빨강으로 지정
		else ofSetColor(ofColor::black); // 그렇지 않다면 그림의 색을 검정으로 지정
		ofDrawCircle(curDot.x, curDot.y, DOT_RADIUS); // 현재 점의 위치에 반지름이 DOT_RADIUS인 원 그리기
		if (i < 10) ofDrawBitmapString(dotNum, curDot.x - DOT_RADIUS / 2, curDot.y - DOT_RADIUS * 1.5); // 적당한 위치에 점의 인덱스 숫자 그리기
		else ofDrawBitmapString(dotNum, curDot.x - DOT_RADIUS, curDot.y - DOT_RADIUS * 1.5); // 위치를 조정하여 점의 인덱스 숫자 그리기
	}

	Dot d1, d2; // 각 직선의 방향을 그리기 위해, 반복문에서 현재 직선의 끝 점을 저장하는 변수
	float t; // 직선 기울기(tan값)를 저장하는 변수
	float c; // cos값을 저장하는 변수
	float s; // sin값을 저장하는 변수
	for (int i = 0; i < dot_amount; i++) // 이중 반복문을 이용하여 각 직선의 방향을 그리기
		for (int j = 0; j < dot_amount; j++)
		{
			if (lineArr[i][j] < INF && lineArr[i][j] != 0) // 현재 처리하는 직선이 존재할 시
			{
				d1 = dotArr[i]; d2 = dotArr[j]; // 현재 직선의 끝 점을 저장

				t = ((float)d2.y - d1.y) / (d2.x - d1.x); // 두 점 사이의 기울기 계산
				c = d2.x - d1.x > 0 ? sqrt(1 / (t * t + 1)) : -sqrt(1 / (t * t + 1)); // 점의 위치에 따른 cos값 계산
				s = d2.y - d1.y > 0 ? sqrt(t * t / (t * t + 1)) : -sqrt(t * t / (t * t + 1)); // 점의 위치에 따른 sin값 계산

				ofSetColor(ofColor::blue); // 그림의 색을 파랑으로 지정
				ofDrawCircle(d2.x - DOT_RADIUS * c, d2.y - DOT_RADIUS * s, DOT_RADIUS / 2); // 계산한 cos값, sin값을 이용하여 직선의 방향에 따른 적당한 위치에 반지름이 4인 원 그리기
			}
		}
}

void ofApp::drawLine()
{
	Dot d1, d2; // 반복문에서 현재 직선의 끝 점을 저장하는 변수

	ofSetLineWidth(LINE_WIDTH); // 직선의 두께를 지정
	ofSetColor(ofColor::gray); // 그림의 색을 회색으로 지정

	for(int i = 0; i < dot_amount; i++) // 이중 반복문을 이용하여 존재하는 모든 직선에 대해 처리
		for (int j = 0; j < dot_amount; j++)
		{
			if (lineArr[i][j] < INF && lineArr[i][j] != 0) // 직선 존재 여부 판단
			{
				d1 = dotArr[i]; d2 = dotArr[j]; // 직선의 끝 점 저장
				ofDrawLine(d1.x, d1.y, d2.x, d2.y); // 끝 점 사이에 직선 그리기
			}
		}
}

void ofApp::drawLineLength()
{
	Dot d1, d2; // 반복문에서 현재 직선의 끝 점을 저장하는 변수
	string lineLen; // 반복문에서 현재 직선의 길이를 문자열로 저장하기 위한 변수

	ofSetColor(ofColor::blue); // 그림의 색을 파랑으로 지정

	for (int i = 0; i < dot_amount; i++) // 이중 반복문을 이용하여 존재하는 모든 직선에 대해 처리
		for (int j = 0; j < dot_amount; j++)
		{
			if (lineArr[i][j] < INF && lineArr[i][j] != 0) // 직선 존재 여부 판단
			{
				d1 = dotArr[i]; d2 = dotArr[j]; // 직선의 끝 점 저장
				lineLen = ofToString(lineArr[i][j]); // ofToString()을 이용하여 직선의 길이를 문자열로 저장
				ofDrawBitmapString(lineLen, (d1.x + d2.x) / 2, (d1.y + d2.y) / 2); // 직선의 끝 점 사이의 중점에 직선의 길이 그리기
			}
		}
}

void ofApp::drawPath()
{
	Dot d1, d2; // 반복문에서 경로에 포함된 직선의 끝 점을 저장하는 변수

	ofSetColor(ofColor::red); // 그림의 색을 빨강으로 지정
	ofSetLineWidth(PATH_WIDTH); // 직선의 두께를 지정

	for (int i = endidx; parent[i] != i && i != -1; i = parent[i]) // 반복문을 이용하여 경로의 마지막 점의 부모를 연속적으로 따라가며, 경로에 포함된 점들 사이에 직선 그리기
	{
		d1 = dotArr[i]; d2 = dotArr[parent[i]]; // 경로에 포함된 직선의 끝 점 저장
		ofDrawLine(d1.x, d1.y, d2.x, d2.y); // 경로에 포함된 직선의 끝 점 사이에 직선 그리기
	}
}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) { // 마우스 클릭이 이루어졌을 시
	clickFlag = true; // 클릭 여부 저장
	isOnceHandled = false; // 클릭 관련 처리 여부 초기화
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	clickFlag = false; // 클릭 여부 저장
}








// 미사용

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
