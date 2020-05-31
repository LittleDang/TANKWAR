#ifndef TW_GAME_H__
#define TW_GAME_H__
#include <vector>
namespace TW {
namespace game {
    struct point2i {
        int x, y;
        point2i()
        {
            x = 0, y = 0;
        }
    };
    class tank {
    public:
        tank();

    private:
        point2i position; //当前坐标
        std::vector<point2i> box; //每一个pixel相对于position 的位移
    };
}
}

#endif