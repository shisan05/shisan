#include <graphics.h>
#include <conio.h>
#include <iostream>
#include <string>
#include <vector>
#include <tchar.h>
#include <windows.h>  // �������ÿ���̨����
#include <algorithm> // �������ͷ�ļ���ȷ��ʹ�� std::max �� std::min

// ���̵ı߽�����
const int BOARD_LEFT = 50;   // ��߽�
const int BOARD_RIGHT = 750; // �ұ߽�
const int BOARD_TOP = 50;    // �ϱ߽�
const int BOARD_BOTTOM = 550; // �±߽�

// ��ҽṹ
struct Player {
    int x, y;   // ��ҵ�ǰλ�õ�����
    int color;  // �����ɫ
    int coins;  // ��ҽ��
    std::wstring name;  // �������
    std::vector<std::wstring> inventory;  // ��ҹ����װ��
};

// װ���ṹ
struct Equipment {
    std::wstring name;
    int price;
};

// ���нṹ
struct City {
    int x, y;   // ���е����Ͻ�����
    std::wstring name;  // ���е�����
};

// ������к�Ұ���������������ƣ�����ʮ�ֲ���
City cities[] = {
    {200, 100, L"���� A"},   // �Ϸ�����
    {300, 200, L"���� B"},   // �ҷ�����
    {200, 300, L"���� C"},   // �·�����
    {100, 200, L"���� D"}    // �󷽳���
};

City wildArea = { 200, 200, L"Ұ������" }; // Ұ������������

// ����װ���б�
Equipment equipments[] = {
    {L"����", 1},
    {L"����", 2},
    {L"��", 3},
    {L"����", 2},
    {L"���", 3},
    {L"ǹ", 4},
    {L"����", 5},
    {L"����", 6},
    {L"̹��", 7},
    {L"�ɻ�", 10},
    {L"��", 3}
};

// ��ʼ��EasyXͼ�δ���
void initGraphics() {
    initgraph(900, 700);  // ���Ӵ��ڴ�С
    setbkcolor(RGB(245, 245, 220));  // ����ǳ��ɫ����
    cleardevice();        // ����
}

// ������Ϸ��ͼ
void drawMap() {
    setfillcolor(RGB(173, 216, 230));
    settextcolor(BLACK);
    setbkmode(TRANSPARENT);
    settextstyle(18, 0, _T("Arial"));

    for (int i = 0; i < 4; i++) {
        solidrectangle(cities[i].x, cities[i].y, cities[i].x + 100, cities[i].y + 100);
        outtextxy(cities[i].x + 25, cities[i].y + 40, cities[i].name.c_str());
    }

    setfillcolor(RGB(144, 238, 144));
    solidrectangle(wildArea.x, wildArea.y, wildArea.x + 100, wildArea.y + 100);
    outtextxy(wildArea.x + 25, wildArea.y + 40, wildArea.name.c_str());
}

// �������
void drawPlayers(std::vector<Player>& players) {
    settextcolor(BLACK);
    settextstyle(16, 0, _T("Arial"));

    for (auto& player : players) {
        setfillcolor(player.color);  // ʹ����ҵ���ɫ
        fillcircle(player.x + 50, player.y + 50, 20);  // �������λ��
        outtextxy(player.x + 25, player.y + 60, player.name.c_str());  // ��ʾ�������
    }
}

// ���Ƶ�ǰ�غ���ʾ
void drawTurnIndicator(Player& currentPlayer) {
    setfillcolor(RGB(255, 228, 181));
    solidrectangle(700, 20, 850, 70);
    settextcolor(BLACK);
    settextstyle(18, 0, _T("Arial"));
    TCHAR turnText[50];
    _stprintf_s(turnText, _T("��ǰ�غ�: %ls"), currentPlayer.name.c_str());
    outtextxy(710, 35, turnText);
}

// �����̵����
void drawShop(Player& player) {
    setfillcolor(RGB(240, 255, 240));  // ʹ��ǳ��ɫ����
    solidrectangle(150, 100, 750, 600);  // �����̵걳��
    settextcolor(BLACK);
    settextstyle(20, 0, _T("Arial"));
    outtextxy(380, 110, _T("�̵� - ѡ��װ��"));

    settextstyle(16, 0, _T("Arial"));
    for (int i = 0; i < 11; i++) {
        TCHAR equipment[100];
        _stprintf_s(equipment, _T("%d. %ls - %d ���"), i + 1, equipments[i].name.c_str(), equipments[i].price);
        outtextxy(200, 150 + i * 30, equipment);
    }

    TCHAR coins[50];
    _stprintf_s(coins, _T("���: %d"), player.coins);
    outtextxy(450, 570, coins);

    setfillcolor(RGB(220, 20, 60));  // ���ùرհ�ťΪ��ɫ
    solidrectangle(600, 550, 700, 590);
    settextcolor(WHITE);
    outtextxy(620, 555, _T("�ر�"));
}

// ���ƿ��ư�ť
void drawControlButtons() {
    setfillcolor(RGB(65, 105, 225));  // �����л��غϰ�ť��ɫ
    solidrectangle(700, 600, 850, 650);
    settextcolor(WHITE);
    settextstyle(20, 0, _T("Arial"));
    outtextxy(710, 615, _T("�л��غ�"));

    setfillcolor(RGB(60, 179, 113));  // �����̵갴ť��ɫ
    solidrectangle(500, 600, 650, 650);
    outtextxy(510, 615, _T("���̵�"));
}

// �л��غϺ���
void switchTurn(int& currentTurn, int numPlayers) {
    currentTurn = (currentTurn + 1) % numPlayers;  // �л�����һ�����
}

// ����̵��ڵĵ��������������͹رղ�����
void handleShopClick(Player& player, int x, int y, bool& shopOpen) {
    for (int i = 0; i < 11; i++) {
        if (x >= 200 && x <= 600 && y >= 150 + i * 30 && y <= 180 + i * 30) {
            if (player.coins >= equipments[i].price) {
                player.coins -= equipments[i].price;
                player.inventory.push_back(equipments[i].name);
                std::wcout << L"�㹺���� " << equipments[i].name << L"!\n";
            }
            else {
                std::wcout << L"��Ҳ��㣡\n";
            }
        }
    }

    if (x >= 600 && x <= 700 && y >= 550 && y <= 590) {
        shopOpen = false;
        std::wcout << L"�̵��ѹرա�\n";
    }
}

// ��鰴ť���
bool checkButtonClick(int x, int y, int left, int top, int right, int bottom) {
    return (x >= left && x <= right && y >= top && y <= bottom);
}

// ���������Ƿ���ĳ�����л�Ұ��������
int checkLocation(int x, int y) {
    for (int i = 0; i < 4; i++) {
        if (x >= cities[i].x && x <= cities[i].x + 100 && y >= cities[i].y && y <= cities[i].y + 100) {
            return i;  // ���س��е�����
        }
    }

    if (x >= wildArea.x && x <= wildArea.x + 100 && y >= wildArea.y && y <= wildArea.y + 100) {
        return 4;  // ����Ұ������ı�־
    }

    return -1;  // û�е���κγ��л�����
}

// ��������ƶ���ȷ�����������̱߽�

// ��������ƶ���ȷ�����������̱߽�
void handlePlayerMovement(Player& player, int targetX, int targetY) {
    // ʹ�� std::max �� std::min ���������������̷�Χ��
    player.x = std::max(BOARD_LEFT, std::min(targetX, BOARD_RIGHT - 100));
    player.y = std::max(BOARD_TOP, std::min(targetY, BOARD_BOTTOM - 100));
}

int main() {
    SetConsoleOutputCP(65001);
    int numPlayers;
    std::vector<Player> players;

    do {
        std::wcout << L"������������� (1-4): ";
        std::cin >> numPlayers;
    } while (numPlayers < 1 || numPlayers > 4);

    for (int i = 0; i < numPlayers; i++) {
        std::wstring playerName;
        std::wcout << L"��������� " << i + 1 << L" ������: ";
        std::wcin >> playerName;
        Player player = { cities[i].x, cities[i].y, RGB(rand() % 256, rand() % 256, rand() % 256), 10, playerName };
        players.push_back(player);
    }

    initGraphics();
    bool shopOpen = false;
    int currentTurn = 0;

    BeginBatchDraw();  // �����������ƣ�������˸

    while (true) {
        cleardevice();
        drawMap();
        drawPlayers(players);
        drawTurnIndicator(players[currentTurn]);
        drawControlButtons();

        if (shopOpen) {
            drawShop(players[currentTurn]);
        }

        if (MouseHit()) {
            MOUSEMSG msg = GetMouseMsg();
            if (msg.uMsg == WM_LBUTTONDOWN) {
                if (shopOpen) {
                    handleShopClick(players[currentTurn], msg.x, msg.y, shopOpen);
                }
                else if (checkButtonClick(msg.x, msg.y, 700, 600, 850, 650)) {
                    switchTurn(currentTurn, numPlayers);
                    shopOpen = false;
                }
                else if (checkButtonClick(msg.x, msg.y, 500, 600, 650, 650)) {
                    int location = checkLocation(players[currentTurn].x, players[currentTurn].y);
                    if (location >= 0 && location < 4) {
                        shopOpen = true;
                    }
                    else {
                        std::wcout << L"�޷���Ұ����̵ꡣ\n";
                    }
                }
                else {
                    handlePlayerMovement(players[currentTurn], msg.x - 50, msg.y - 50); // �����λ���ƶ�
                }
            }
        }

        FlushBatchDraw();  // ����������ƣ�ˢ����Ļ��ʾ
        Sleep(10);  // ��ʱ������ 10 ms �������Ӧ�ٶ�
    }

    EndBatchDraw();  // �ر���������
    closegraph();
    return 0;
}
