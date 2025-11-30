#include <windows.h>
#include <string>
#include <iostream>
using namespace std;
string GetClipboardText()
{
    if (!OpenClipboard(nullptr))
    {
        return "";
    }//如果不成功打开剪贴板返回空字符串
    HANDLE hData = GetClipboardData(CF_TEXT);//句柄,详询deepseek,其实是一种类似于指针的变量
    if (!hData)
    {
        CloseClipboard();
        return "";
    }//如果没有数据返回空字符串
    char* pszText = static_cast<char*>(GlobalLock(hData));//锁定内存获取指针,这里(windows api)允许globallock返回的值cast为cpp内的指针
    string text(pszText);//初始化
    GlobalUnlock(hData);//解锁内存,详询deepseek,windows api在操作内存时需要锁定和解锁,防止其他程序修改,这里就是用到就锁定它,用不到就解锁它
    CloseClipboard();
    return text;
}
void TypeString(string text)//模拟键盘输入
{
    for (char c : text)
    {
        if (c == '\n')//处理特殊字符如换行、制表符等
        {
            keybd_event(VK_RETURN, 0, 0, 0);//按下回车键
            keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);//释放回车键
        }
        else
        {
            SHORT vk = VkKeyScanA(c);//获取虚拟键码和修饰符,注意我们的循环对象是c,每次丢到这个循环里的都是一个c,这里把c转换成它对应的虚拟键码和它的修饰符状态,可以理解成vk是一个结构体
            BYTE vkCode = LOBYTE(vk);//获取虚拟键码,即获取结构体内的低字节,不过这里不明说c是一个结构体,只是为了方便理解,自己需要知道它的低字节是虚拟键码,高字节是修饰符状态
            BYTE shiftState = HIBYTE(vk);//获取修饰符状态,即获取结构体内的高字节,不过为什么c不是结构体那么直观?windows设计如此,前程序员烂程序
            if (shiftState & 1)//处理Shift键
            {
                keybd_event(VK_SHIFT, 0, 0, 0);//按下Shift键,第一个参数是虚拟键码,第二个参数是扫描码,第三个参数是标志位,第四个参数是附加信息,这里不需要所以都设为0,2和4现在基本都用不到(我问了deepseek),3可以定义按下释放
            }
            keybd_event(vkCode, 0, 0, 0);//按下对应键
            keybd_event(vkCode, 0, KEYEVENTF_KEYUP, 0);//释放对应键
            if (shiftState & 1)
            {
                keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);//释放Shift键
            }
        }
        Sleep(10);//适当延迟避免输入过快,根据实际情况调整
    }
}
int main()//简单逻辑不再解释
{
    cout << "请在5秒内切换到文本编辑器......" << endl;
    Sleep(5000);
    string code = GetClipboardText();
    if (code.empty())
    {
        cout << "剪贴板为空或读取失败!" << endl;
        return 1;
    }
    cout << "开始自动输入......" << endl;
    TypeString(code);
    cout << "输入完成!" << endl;
    return 0;
}