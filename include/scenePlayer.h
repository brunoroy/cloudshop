#ifndef SCENEPLAYER_H
#define SCENEPLAYER_H

#include "config.h"

class ScenePlayer
{
public:
    ScenePlayer(const uint frameCount = 0);
    ~ScenePlayer();

    void init(const uint frameCount);
    void play();
    void pause();

    bool isPaused();
    bool isReady();
    bool isFirstFrame();
    bool isLastFrame();
    bool frameHasChanged();

    void setFrameCount(const uint frameCount);

    unsigned int getFrameCount();
    unsigned int getCurrentFrame();
    unsigned int getNextFrame();
    unsigned int getPreviousFrame();
    unsigned int getFirstFrame();

    void setCurrentFrame(const uint currentFrame);

private:
    bool _isPaused;
    uint _currentFrame;
    uint _previousFrame;
    uint _firstFrame;
    uint _lastFrame;
    uint _frameCount;
};

#endif // SCENEPLAYER_H
