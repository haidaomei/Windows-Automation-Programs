#include <Windows.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <conio.h>
#include <thread>
#include <numeric>
#include <limits>
using namespace std;
bool Index = false;
void SimulateKeyPress(WORD KeyCode)//这是虚拟键码,就是按下哪个键的意思
{
    INPUT input = { 0 };//这里INPUT是一个嵌套结构体类,包含MOUSEINPUT结构体类对象mi、KEYBDINPUT结构体类对象ki、HARDWAREINPUT结构体类对象hi等属性,windows应该是把这些属性整合到一起耦合成一个输入信号
    input.type = INPUT_KEYBOARD;//设置输入类型为键盘输入,INPUT_KEYBOARD是预定义常量(值为1),表示这是一个键盘事件
    input.ki.wVk = KeyCode;//INPUT结构体类对象input里的KEYBDINPUT结构体类对象ki里的WORD类对象KeyCode,表示按下哪个键
    input.ki.dwFlags = 0;//简单地说,input信号内的键盘信号内的键的属性是按下还是抬起(和上一句的哪个键是平行的,不是包含的,这里微软如果定义按下抬起在按下哪个键内确实带来很大冗余,平行定义还有相同实现,还节省了定义空间)
    SendInput(1, &input, sizeof(INPUT));//进行按下事件的传输
    this_thread::sleep_for(chrono::milliseconds(50));//模拟按下再放松的过程,chrono是用来精准计时的,这来自cpp11特性,不是windowsapi
    input.ki.dwFlags = KEYEVENTF_KEYUP;//进行松开事件的传输,注意按下松开windows一般作两个事件来处理
    SendInput(1, &input, sizeof(INPUT));//这就是向系统发送输入事件的函数,第一个参代表发送数量,第二个参代表事件,第三个参
}
void SimulateMouseRightClick()//逻辑和上面相同
{
    INPUT input = { 0 };
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
    SendInput(1, &input, sizeof(INPUT));//按下右键
    this_thread::sleep_for(std::chrono::milliseconds(50));//话说回来,thisthread是cpp11里面一组允许进程自我管理的api
    input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
    SendInput(1, &input, sizeof(INPUT));
}
void MainLoop1()//火龙果
{
    int seconds = 5;
    auto gap = chrono::seconds(seconds);
    while (!Index)
    {
        cout << seconds << "秒钟循环" << endl;
        SimulateKeyPress(VK_SPACE);
        this_thread::sleep_for(chrono::milliseconds(250));
        SimulateMouseRightClick();//这两句正式执行操作
        auto start = chrono::steady_clock::now();//同样是时钟,这程序要用到thread库还挺多的,没学过估计做不出,这里不讲用法,看下面while循环就懂了
        while (!Index)
        {
            this_thread::sleep_for(chrono::milliseconds(100));//这玩意是必须的,控制循环的时钟速度,不然高速运行电脑顶不住,同时时间也不能设得太长,如果想退出该程序(由掌控线程1的语句执行),那么这while(掌握线程2)需要很长时间才能跳转线程1
            auto end = chrono::steady_clock::now();
            if (end - start >= gap)
            {
                break;
            }
        }
    }
}
void MainLoop2()//石神、死神、圣水
{
    int seconds = 5;
    auto gap = chrono::seconds(seconds);
    while (!Index)
    {
        cout << seconds << "秒钟循环" << endl;
        SimulateMouseRightClick();
        auto start = chrono::steady_clock::now();
        while (!Index)
        {
            this_thread::sleep_for(chrono::milliseconds(100));
            auto end = chrono::steady_clock::now();
            if (end - start >= gap)
            {
                break;
            }
        }
    }
}
void MainLoop3()
{
    int seconds = 5;
    auto gap = chrono::seconds(seconds);
    while (!Index)
    {
        cout << seconds << "秒钟循环" << endl;
        SimulateKeyPress(VK_OEM_2);
        this_thread::sleep_for(chrono::milliseconds(25));
        SimulateKeyPress('H');
        this_thread::sleep_for(chrono::milliseconds(25));
        SimulateKeyPress('E');
        this_thread::sleep_for(chrono::milliseconds(25));
        SimulateKeyPress('A');
        this_thread::sleep_for(chrono::milliseconds(25));
        SimulateKeyPress('L');
        this_thread::sleep_for(chrono::milliseconds(25));
        SimulateKeyPress(VK_RETURN);
        auto start = chrono::steady_clock::now();
        while (!Index)
        {
            this_thread::sleep_for(chrono::milliseconds(100));
            auto end = chrono::steady_clock::now();
            if (end - start >= gap)
            {
                break;
            }
        }
    }
}
void KeyListener()//检测键盘输入
{
    while (!Index)
    {
        if (GetAsyncKeyState(VK_ESCAPE))
        {
            Index = true;
        }
    }
}
int main(void)
{
    int number;
    bool validInput = false;
    while (!validInput)
    {
        cout << "1:火龙果;2:石神/死神/圣水;3:/heal" << endl;
        cin >> number;
        if (cin.fail() || (number < 1 || number > 3))
        {
            cin.clear();
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');//这里byd的会有命名空间污染 我也是第一次见到了
            cout << "格式错误,重新输入" << endl;
        }
        else
        {
            validInput = true;
        }
    }
    if (number == 1)
    {
        cout << "按ESC键退出" << endl;
        thread listener(KeyListener);//创建并启动监听线程
        MainLoop1();
        listener.join();//进程结束,注意这是结束不是加,大概逻辑就是我在上去两行开了一个处理通道用于输入键盘事件,上去一行开了另一个通道来处理主逻辑,我按下esc可以中断第二个通道的进行然后顺位中断第一个通道进行
    }
    else if (number == 2)
    {
        cout << "按ESC键退出" << endl;
        thread listener(KeyListener);
        MainLoop2();
        listener.join();
    }
    else
    {
        cout << "按ESC键退出" << endl;
        thread listener(KeyListener);
        MainLoop3();
        listener.join();
    }
    cout << "程序已退出" << endl;
    return 0;
}