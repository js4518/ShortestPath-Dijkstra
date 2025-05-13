#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	// â �̸� ����
	ofSetWindowTitle("Shortest Path Finder");

	// ȭ�� �� ����
	ofBackground(ofColor::white);

	// â �޴� �߰�
	hWnd = WindowFromDC(wglGetCurrentDC()); // ���� �ٷ�� â �ҷ�����
	menu = new ofxWinMenu(this, hWnd); // �޴� ������Ʈ�� ���ο� �޴��� �Ҵ�
	menu->CreateMenuFunction(&ofApp::appMenuFunction); // �޴��� ���� �۵��� �Լ� ����
	HMENU hMenu = menu->CreateWindowMenu(); // ���� â�� �޴� �߰�
	HMENU hPopup = menu->AddPopupMenu(hMenu, "���"); // �޴��� "���" �˾� �߰�
	menu->AddPopupItem(hPopup, "�� �׸���", false, false); // �˾��� "�� �׸���" ��� �߰�
	menu->AddPopupItem(hPopup, "���� �׸���", false, false); // �˾��� "���� �׸���" ��� �߰�
	menu->AddPopupItem(hPopup, "�ִ� ��� ã��", false, false); // �˾��� "�ִ� ��� ã��" ��� �߰�
	menu->AddPopupItem(hPopup, "����", false, false); // �˾��� "����" ��� �߰�
	menu->SetWindowMenu(); // �տ��� �����Ѵ�� â�� �޴� ����
	
	// ���� �ʱ�ȭ
	isDot = false;
	isLine = false;
	isPath = false;
	clickFlag = false;
	isOnceHandled = false;
	dot_amount = 0;
	selected_amount = 0;
	endidx = -1;

	Dot newDot; // selected �迭�� �ʱ�ȭ�� ���� Dot Ÿ�� ������ ���� ����
	newDot.idx = -1;
	selected[0] = newDot; selected[1] = newDot; // selected �迭 �ʱ�ȭ

	for (int i = 0; i < MAX_DOTS; i++) // ���� �ݺ����� �̿��Ͽ� lineArr �迭 �ʱ�ȭ
		for (int j = 0; j < MAX_DOTS; j++)
		{
			if (i == j) lineArr[i][j] = 0; // �ڱ� �ڽ������� ���� ���̸� 0���� ����
			else lineArr[i][j] = INF; // �׷��� �ʴٸ� ���Ѵ�� ����
		}
}

void ofApp::appMenuFunction(string title, bool bChecked) 
{
	if (title == "�� �׸���")  // "�� �׸���" ����� ���õ� ���
	{
		// isDot�� true�� �Ͽ� ���õǾ��ٴ� ���� ����
		// ���� ������ ����� ��Ȱ��ȭ �ϱ� ���� isLine, isPath�� false�� �Ѵ�
		isDot = true;
		isLine = false;
		isPath = false;
	}
	if (title == "���� �׸���") // "���� �׸���" ����� ���õ� ���
	{
		// ���� ����
		isDot = false;
		isLine = true;
		isPath = false;
	}
	if (title == "�ִ� ��� ã��") // "�ִ� ��� ã��" ����� ���õ� ���
	{
		// ���� ����
		isDot = false;
		isLine = false;
		isPath = true;

		// �ݺ����� �̿��Ͽ� parent �迭�� �ʱ�ȭ(+ ������ ����� ��θ� �����)
		for (int i = 0; i < dot_amount; i++) 
			parent[i] = i; // �� ���� �θ� �ڱ� �ڽ����� ����
		endidx = -1; // ����� �� �� ���� �ʱ�ȭ
	}
	if(title == "����") ofExit(); // "����" ����� ���õ� ��� ���α׷� ����
}


//--------------------------------------------------------------
void ofApp::update(){
	if (isDot) // "�� �׸���" ����� ���õ� ���
		addDot(); // ���� �߰��ϴ� �Լ��� ȣ��
	else if (isLine || isPath) // "���� �׸���" �Ǵ� "�ֵ� ��� ã��" ����� ���õ� ���
		selectDot(); // ���� �����ϴ� �Լ��� ȣ��
}

//--------------------------------------------------------------
void ofApp::draw(){
	string pLen; // ����� ���̸� ���ڿ� ���·� �����ϴ� ����

	ofSetColor(ofColor::black); // �׸��� ���� �������� ����
	ofDrawBitmapString("Click -> Draw Dot / Select Dot", 30, 30); // �ȳ� ���� �׸���

	if (isPath && endidx != -1) // "�ִ� ��� ã��" ����� ���õǾ���, ��� Ž���� �̷������ ���
		if (pathLen[endidx] != INF) // ���� ��ΰ� �ùٸ��� ã������ ���(���õ� �� ���̿� ��ΰ� ����)
		{
			pLen = "Path Length : " + ofToString(pathLen[endidx]); // ofToString()�� �̿��Ͽ� ����� ���̸� ���ڿ��� �ٲٰ� pLen�� ����
			ofDrawBitmapString(pLen, 30, 60); // ����� ���� �׸���
		}

	drawLine(); // ������ �׸��� �Լ��� ȣ��
	drawDot(); // ���� �׸��� �Լ��� ȣ��
	if (isPath) drawPath(); // "�ִ� ��� ã��" ����� ���õǾ��ٸ� ��θ� �׸��� �Լ� ȣ��
	drawLineLength();// ������ ���̸� �׸��� �Լ� ȣ��
}

void ofApp::addDot()
{
	int x, y; // ���콺�� ��ġ�� �����ϴ� ����
	Dot newDot; // �迭�� ���ο� ���� ������ �߰��ϱ� ���� ����

	if (clickFlag && !isOnceHandled) // Ŭ���� �̷������ ���� Ŭ�� ���� ó���� �̷������ �ʾ��� ��
	{
		if (dot_amount != MAX_DOTS) // ���� ���� ������ �ִ뿡 �����Ͽ� ���̻� ���� �׸��� ���ϴ� ��Ȳ�� �ƴ� ��
		{
			x = ofGetMouseX(); y = ofGetMouseY(); // ���콺 ��ġ�� ����
			newDot.x = x; newDot.y = y; // ���ο� ���� ���� Dot ��ü�� ��ġ ����
			newDot.idx = dot_amount; // �ε����� ����
			dotArr[dot_amount++] = newDot; // dotArr �迭�� ���� ����
		}
		else printf("�� ������ �ִ��Դϴ�.\n"); // �ȳ� �޼��� ���
		isOnceHandled = true; // Ŭ�� ���� ó���� �̷�������� ����
	}
}

void ofApp::selectDot()
{
	int x, y; // ���콺�� ��ġ�� �����ϴ� ����
	int dis; // ���콺�� ��ġ�� �� ������ �Ÿ��� �����ϴ� ����
	Dot curDot; // �ݺ������� ���� ���� ������ �����ϴ� ����

	if (clickFlag && !isOnceHandled) // Ŭ���� �̷������ ���� Ŭ�� ���� ó���� �̷������ �ʾ��� ��
	{
		x = ofGetMouseX(), y = ofGetMouseY(); // ���콺 ��ġ�� ����
		for (int i = 0; i < dot_amount; i++) // �ݺ����� �̿��Ͽ� ���� ���콺�� ��ġ�� �� ���� ���� �ִ��� ����
		{
			curDot = dotArr[i]; // ���� ���� ������ ����
			dis = (x - curDot.x) * (x - curDot.x) + (y - curDot.y) * (y - curDot.y); // ���� �Ÿ� ���
			if (dis < DOT_RADIUS * DOT_RADIUS)
			{
				selected[selected_amount++] = curDot; // ���콺�� ��ġ�� ���� ���� ������ �� ���� selected �迭�� ����
				break; // �� ���� ���ؼ��� ó��
			}
		}
		if (selected_amount == 2) // ���õ� ���� ������ 2���� ��
		{
			selected_amount = 0; // ���õ� ���� ���� �ʱ�ȭ
			if (isLine) addLine(); // "���� �׸���" ����� ���õ� ��Ȳ�̶�� ���õ� �� �� ���̿� ������ �׸��� �Լ� ȣ��
			if (isPath) calculatePath(); // "�ִ� ��� ã��" ����� ���õ� ��Ȳ�̶�� ���õ� �� �� ������ �ִ� ��θ� Ž���ϴ� �Լ� ȣ��
		}
		isOnceHandled = true; // Ŭ�� ���� ó���� �̷�������� ����
	}
}

void ofApp::addLine()
{
	Dot d1 = selected[0], d2 = selected[1]; // ���� ���õ� ���� ������ �����ϴ� ����
	Dot newDot; // selected �迭 �ʱ�ȭ�� ���� ����
	newDot.idx = -1;
	selected[0] = newDot; selected[1] = newDot; // selected �迭 �ʱ�ȭ

	if (d1.idx == d2.idx) return; // ������ ���� �� �� ���õǾ��� �� ó�� X

	int dis = sqrt((d1.x - d2.x) * (d1.x - d2.x) + (d1.y - d2.y) * (d1.y - d2.y)); // ���õ� �� �� ������ �Ÿ� ���
	lineArr[d1.idx][d2.idx] = dis; // lineArr �迭�� ���������� ���� ������ �Ÿ� ����
}

void ofApp::calculatePath()
{
	int d1 = selected[0].idx, d2 = selected[1].idx; // ���� ���õ� ���� ������ �����ϴ� ����
	int curd; // �ݺ�������, ���� �湮���� �ʾҰ� �������� ���� ������ �ִ� ��(�ִܰŸ� ��)�� �ε����� �����ϴ� ����
	int minLen; // ���� ������ �ִ� ���� ã�� ���� ���� �ݺ������� �ִܰŸ��� �����ϴ� ����

	Dot newDot; // selected �迭 �ʱ�ȭ�� ���� ����
	newDot.idx = -1;
	selected[0] = newDot; selected[1] = newDot; // selected �迭 �ʱ�ȭ

	if (d1 == d2) return; // ������ ���� �� �� ���õǾ��� �� ó�� X

	for (int i = 0; i < dot_amount; i++) // �ݺ����� �̿��Ͽ� �� ���� ���� �ʱ�ȭ
	{
		found[i] = false; // ���� ���� �湮 ���θ� �ʱ�ȭ
		pathLen[i] = lineArr[d1][i]; // ���������� ���� �������� �Ÿ� ���� 
		parent[i] = i; // ���� ���� �θ� ������ �ʱ�ȭ(�ڱ� �ڽ����� ����)
		if (pathLen[i] < INF) parent[i] = d1; // ���� ���� �������� �̾����ִٸ� ���� ���� �θ� ���������� ����
	}
	found[d1] = true; // �������� �湮 ���� ����

	for (int i = 0; i < dot_amount - 2; i++) // �ݺ����� �̿��Ͽ� (�� ����-2)����ŭ ó���� �ݺ��Ͽ� ���������� ��� �������� ��� �Ÿ� ���
	{
		minLen = INF; // �ִܰŸ� �ʱ�ȭ
		curd = -1; // �ִܰŸ� �� �ʱ�ȭ
		for (int j = 0; j < dot_amount; j++) // �ݺ����� �̿��Ͽ�, �湮���� �ʾҰ� �������� ���� ������ �ִ� ��(�ִܰŸ� ��) ã��
			if (pathLen[j] < minLen && !found[j]) // �������� ����Ǿ����� ���� ���� �Ÿ� �񱳿��� Ż��
			{
				minLen = pathLen[j];
				curd = j;
			}
		
		if (curd == -1) break; // �������� ����Ǿ��ִ� ���� �湮���� ���� ���� ������ ����

		found[curd] = true; // �ִܰŸ� ���� �湮 ���� ���� 
		for (int j = 0; j < dot_amount; j++) // �ݺ����� �̿��Ͽ�, �ִܰŸ� ���� ���� �湮���� ���� �� ���̿� ���� ó��
			if (!found[j]) // �湮 ���� �Ǵ�
				if (pathLen[curd] + lineArr[curd][j] < pathLen[j]) // "���������� �ִܰŸ� �������� ��� �Ÿ� + �ִܰŸ� ������ ���� �������� �Ÿ� < ���������� ���� �������� ��� �Ÿ�"�Ͻ�
				{
					pathLen[j] = pathLen[curd] + lineArr[curd][j]; // ���������� ���� �������� ��� �Ÿ� ������Ʈ
					parent[j] = curd; // ���� ���� �θ� �ִܰŸ� ������ ����
				}
	}

	endidx = d2; // ������ �ε��� ����
}

void ofApp::drawDot()
{
	Dot curDot; // �ݺ������� ���� ���� ������ �����ϴ� ����
	string dotNum; // �ݺ������� ���� ���� �ε��� ���ڸ� ���ڿ��� �����ϱ� ���� ����

	for (int i = 0; i < dot_amount; i++) // �ݺ����� �̿��Ͽ� dotArr �迭�� ����� ��� ���� �׸���
	{
		curDot = dotArr[i]; // curDot�� ���� ���� ���� ����
		dotNum = ofToString(i); // ofToString()�� �̿��Ͽ� ���� ���� �ε����� ���ڿ��� ����
		if (curDot.idx == selected[0].idx || curDot.idx == selected[1].idx) ofSetColor(ofColor::red); // ���� ���� ���õ� ���̸� �׸��� ���� �������� ����
		else ofSetColor(ofColor::black); // �׷��� �ʴٸ� �׸��� ���� �������� ����
		ofDrawCircle(curDot.x, curDot.y, DOT_RADIUS); // ���� ���� ��ġ�� �������� DOT_RADIUS�� �� �׸���
		if (i < 10) ofDrawBitmapString(dotNum, curDot.x - DOT_RADIUS / 2, curDot.y - DOT_RADIUS * 1.5); // ������ ��ġ�� ���� �ε��� ���� �׸���
		else ofDrawBitmapString(dotNum, curDot.x - DOT_RADIUS, curDot.y - DOT_RADIUS * 1.5); // ��ġ�� �����Ͽ� ���� �ε��� ���� �׸���
	}

	Dot d1, d2; // �� ������ ������ �׸��� ����, �ݺ������� ���� ������ �� ���� �����ϴ� ����
	float t; // ���� ����(tan��)�� �����ϴ� ����
	float c; // cos���� �����ϴ� ����
	float s; // sin���� �����ϴ� ����
	for (int i = 0; i < dot_amount; i++) // ���� �ݺ����� �̿��Ͽ� �� ������ ������ �׸���
		for (int j = 0; j < dot_amount; j++)
		{
			if (lineArr[i][j] < INF && lineArr[i][j] != 0) // ���� ó���ϴ� ������ ������ ��
			{
				d1 = dotArr[i]; d2 = dotArr[j]; // ���� ������ �� ���� ����

				t = ((float)d2.y - d1.y) / (d2.x - d1.x); // �� �� ������ ���� ���
				c = d2.x - d1.x > 0 ? sqrt(1 / (t * t + 1)) : -sqrt(1 / (t * t + 1)); // ���� ��ġ�� ���� cos�� ���
				s = d2.y - d1.y > 0 ? sqrt(t * t / (t * t + 1)) : -sqrt(t * t / (t * t + 1)); // ���� ��ġ�� ���� sin�� ���

				ofSetColor(ofColor::blue); // �׸��� ���� �Ķ����� ����
				ofDrawCircle(d2.x - DOT_RADIUS * c, d2.y - DOT_RADIUS * s, DOT_RADIUS / 2); // ����� cos��, sin���� �̿��Ͽ� ������ ���⿡ ���� ������ ��ġ�� �������� 4�� �� �׸���
			}
		}
}

void ofApp::drawLine()
{
	Dot d1, d2; // �ݺ������� ���� ������ �� ���� �����ϴ� ����

	ofSetLineWidth(LINE_WIDTH); // ������ �β��� ����
	ofSetColor(ofColor::gray); // �׸��� ���� ȸ������ ����

	for(int i = 0; i < dot_amount; i++) // ���� �ݺ����� �̿��Ͽ� �����ϴ� ��� ������ ���� ó��
		for (int j = 0; j < dot_amount; j++)
		{
			if (lineArr[i][j] < INF && lineArr[i][j] != 0) // ���� ���� ���� �Ǵ�
			{
				d1 = dotArr[i]; d2 = dotArr[j]; // ������ �� �� ����
				ofDrawLine(d1.x, d1.y, d2.x, d2.y); // �� �� ���̿� ���� �׸���
			}
		}
}

void ofApp::drawLineLength()
{
	Dot d1, d2; // �ݺ������� ���� ������ �� ���� �����ϴ� ����
	string lineLen; // �ݺ������� ���� ������ ���̸� ���ڿ��� �����ϱ� ���� ����

	ofSetColor(ofColor::blue); // �׸��� ���� �Ķ����� ����

	for (int i = 0; i < dot_amount; i++) // ���� �ݺ����� �̿��Ͽ� �����ϴ� ��� ������ ���� ó��
		for (int j = 0; j < dot_amount; j++)
		{
			if (lineArr[i][j] < INF && lineArr[i][j] != 0) // ���� ���� ���� �Ǵ�
			{
				d1 = dotArr[i]; d2 = dotArr[j]; // ������ �� �� ����
				lineLen = ofToString(lineArr[i][j]); // ofToString()�� �̿��Ͽ� ������ ���̸� ���ڿ��� ����
				ofDrawBitmapString(lineLen, (d1.x + d2.x) / 2, (d1.y + d2.y) / 2); // ������ �� �� ������ ������ ������ ���� �׸���
			}
		}
}

void ofApp::drawPath()
{
	Dot d1, d2; // �ݺ������� ��ο� ���Ե� ������ �� ���� �����ϴ� ����

	ofSetColor(ofColor::red); // �׸��� ���� �������� ����
	ofSetLineWidth(PATH_WIDTH); // ������ �β��� ����

	for (int i = endidx; parent[i] != i && i != -1; i = parent[i]) // �ݺ����� �̿��Ͽ� ����� ������ ���� �θ� ���������� ���󰡸�, ��ο� ���Ե� ���� ���̿� ���� �׸���
	{
		d1 = dotArr[i]; d2 = dotArr[parent[i]]; // ��ο� ���Ե� ������ �� �� ����
		ofDrawLine(d1.x, d1.y, d2.x, d2.y); // ��ο� ���Ե� ������ �� �� ���̿� ���� �׸���
	}
}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) { // ���콺 Ŭ���� �̷������ ��
	clickFlag = true; // Ŭ�� ���� ����
	isOnceHandled = false; // Ŭ�� ���� ó�� ���� �ʱ�ȭ
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	clickFlag = false; // Ŭ�� ���� ����
}








// �̻��

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
