#include <graphics.h>
#include <conio.h>
#include <iostream>
#include <string>
#include <vector>
#include <tchar.h>
#include <windows.h>  // 用于设置控制台编码
#include <algorithm> // 加入这个头文件以确保使用 std::max 和 std::min

// 棋盘的边界限制
const int BOARD_LEFT = 50;   // 左边界
const int BOARD_RIGHT = 750; // 右边界
const int BOARD_TOP = 50;    // 上边界
const int BOARD_BOTTOM = 550; // 下边界

// 玩家结构
struct Player {
    int x, y;   // 玩家当前位置的坐标
    int color;  // 玩家颜色
    int coins;  // 玩家金币
    std::wstring name;  // 玩家名字
    std::vector<std::wstring> inventory;  // 玩家购买的装备
};

// 装备结构
struct Equipment {
    std::wstring name;
    int price;
};

// 城市结构
struct City {
    int x, y;   // 城市的左上角坐标
    std::wstring name;  // 城市的名称
};

// 定义城市和野外区域的坐标和名称，按照十字布局
City cities[] = {
    {200, 100, L"城市 A"},   // 上方城市
    {300, 200, L"城市 B"},   // 右方城市
    {200, 300, L"城市 C"},   // 下方城市
    {100, 200, L"城市 D"}    // 左方城市
};

City wildArea = { 200, 200, L"野外区域" }; // 野外区域在中心

// 定义装备列表
Equipment equipments[] = {
    {L"铁剑", 1},
    {L"银剑", 2},
    {L"金剑", 3},
    {L"银盾", 2},
    {L"金盾", 3},
    {L"枪", 4},
    {L"手榴弹", 5},
    {L"大炮", 6},
    {L"坦克", 7},
    {L"飞机", 10},
    {L"马", 3}
};

// 初始化EasyX图形窗口
void initGraphics() {
    initgraph(900, 700);  // 增加窗口大小
    setbkcolor(RGB(245, 245, 220));  // 设置浅米色背景
    cleardevice();        // 清屏
}

// 绘制游戏地图
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

// 绘制玩家
void drawPlayers(std::vector<Player>& players) {
    settextcolor(BLACK);
    settextstyle(16, 0, _T("Arial"));

    for (auto& player : players) {
        setfillcolor(player.color);  // 使用玩家的颜色
        fillcircle(player.x + 50, player.y + 50, 20);  // 绘制玩家位置
        outtextxy(player.x + 25, player.y + 60, player.name.c_str());  // 显示玩家名字
    }
}

// 绘制当前回合提示
void drawTurnIndicator(Player& currentPlayer) {
    setfillcolor(RGB(255, 228, 181));
    solidrectangle(700, 20, 850, 70);
    settextcolor(BLACK);
    settextstyle(18, 0, _T("Arial"));
    TCHAR turnText[50];
    _stprintf_s(turnText, _T("当前回合: %ls"), currentPlayer.name.c_str());
    outtextxy(710, 35, turnText);
}

// 绘制商店界面
void drawShop(Player& player) {
    setfillcolor(RGB(240, 255, 240));  // 使用浅绿色背景
    solidrectangle(150, 100, 750, 600);  // 绘制商店背景
    settextcolor(BLACK);
    settextstyle(20, 0, _T("Arial"));
    outtextxy(380, 110, _T("商店 - 选择装备"));

    settextstyle(16, 0, _T("Arial"));
    for (int i = 0; i < 11; i++) {
        TCHAR equipment[100];
        _stprintf_s(equipment, _T("%d. %ls - %d 金币"), i + 1, equipments[i].name.c_str(), equipments[i].price);
        outtextxy(200, 150 + i * 30, equipment);
    }

    TCHAR coins[50];
    _stprintf_s(coins, _T("金币: %d"), player.coins);
    outtextxy(450, 570, coins);

    setfillcolor(RGB(220, 20, 60));  // 设置关闭按钮为红色
    solidrectangle(600, 550, 700, 590);
    settextcolor(WHITE);
    outtextxy(620, 555, _T("关闭"));
}

// 绘制控制按钮
void drawControlButtons() {
    setfillcolor(RGB(65, 105, 225));  // 设置切换回合按钮颜色
    solidrectangle(700, 600, 850, 650);
    settextcolor(WHITE);
    settextstyle(20, 0, _T("Arial"));
    outtextxy(710, 615, _T("切换回合"));

    setfillcolor(RGB(60, 179, 113));  // 设置商店按钮颜色
    solidrectangle(500, 600, 650, 650);
    outtextxy(510, 615, _T("打开商店"));
}

// 切换回合函数
void switchTurn(int& currentTurn, int numPlayers) {
    currentTurn = (currentTurn + 1) % numPlayers;  // 切换到下一个玩家
}

// 检查商店内的点击（处理点击购买和关闭操作）
void handleShopClick(Player& player, int x, int y, bool& shopOpen) {
    for (int i = 0; i < 11; i++) {
        if (x >= 200 && x <= 600 && y >= 150 + i * 30 && y <= 180 + i * 30) {
            if (player.coins >= equipments[i].price) {
                player.coins -= equipments[i].price;
                player.inventory.push_back(equipments[i].name);
                std::wcout << L"你购买了 " << equipments[i].name << L"!\n";
            }
            else {
                std::wcout << L"金币不足！\n";
            }
        }
    }

    if (x >= 600 && x <= 700 && y >= 550 && y <= 590) {
        shopOpen = false;
        std::wcout << L"商店已关闭。\n";
    }
}

// 检查按钮点击
bool checkButtonClick(int x, int y, int left, int top, int right, int bottom) {
    return (x >= left && x <= right && y >= top && y <= bottom);
}

// 检查鼠标点击是否在某个城市或野外区域内
int checkLocation(int x, int y) {
    for (int i = 0; i < 4; i++) {
        if (x >= cities[i].x && x <= cities[i].x + 100 && y >= cities[i].y && y <= cities[i].y + 100) {
            return i;  // 返回城市的索引
        }
    }

    if (x >= wildArea.x && x <= wildArea.x + 100 && y >= wildArea.y && y <= wildArea.y + 100) {
        return 4;  // 返回野外区域的标志
    }

    return -1;  // 没有点击任何城市或区域
}

// 处理玩家移动，确保不超出棋盘边界

// 处理玩家移动，确保不超出棋盘边界
void handlePlayerMovement(Player& player, int targetX, int targetY) {
    // 使用 std::max 和 std::min 来限制坐标在棋盘范围内
    player.x = std::max(BOARD_LEFT, std::min(targetX, BOARD_RIGHT - 100));
    player.y = std::max(BOARD_TOP, std::min(targetY, BOARD_BOTTOM - 100));
}

int main() {
    SetConsoleOutputCP(65001);
    int numPlayers;
    std::vector<Player> players;

    do {
        std::wcout << L"请输入玩家人数 (1-4): ";
        std::cin >> numPlayers;
    } while (numPlayers < 1 || numPlayers > 4);

    for (int i = 0; i < numPlayers; i++) {
        std::wstring playerName;
        std::wcout << L"请输入玩家 " << i + 1 << L" 的名字: ";
        std::wcin >> playerName;
        Player player = { cities[i].x, cities[i].y, RGB(rand() % 256, rand() % 256, rand() % 256), 10, playerName };
        players.push_back(player);
    }

    initGraphics();
    bool shopOpen = false;
    int currentTurn = 0;

    BeginBatchDraw();  // 启动批量绘制，减少闪烁

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
                        std::wcout << L"无法在野外打开商店。\n";
                    }
                }
                else {
                    handlePlayerMovement(players[currentTurn], msg.x - 50, msg.y - 50); // 鼠标点击位置移动
                }
            }
        }

        FlushBatchDraw();  // 完成批量绘制，刷新屏幕显示
        Sleep(10);  // 延时降低至 10 ms 以提高响应速度
    }

    EndBatchDraw();  // 关闭批量绘制
    closegraph();
    return 0;
}
