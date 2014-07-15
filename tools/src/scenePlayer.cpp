#include "scenePlayer.h"
#include <iostream>

ScenePlayer::ScenePlayer(const uint frameCount):
    _isPaused(true)
{
    init(frameCount);
}

ScenePlayer::~ScenePlayer()
{
}

void ScenePlayer::init(const uint frameCount)
{
    _frameCount = frameCount;
    _firstFrame = 0;
    _lastFrame = frameCount-1;
    _previousFrame = _firstFrame;
    _currentFrame = _firstFrame;
}

void ScenePlayer::setFrameCount(const uint frameCount)
{
    init(frameCount);
}

void ScenePlayer::play()
{
    _isPaused = false;
}

void ScenePlayer::pause()
{
    _isPaused = true;
}

bool ScenePlayer::isPaused()
{
    return _isPaused;
}

unsigned int ScenePlayer::getFrameCount()
{
    return _frameCount;
}

unsigned int ScenePlayer::getCurrentFrame()
{
    return _currentFrame;
}

void ScenePlayer::setCurrentFrame(const unsigned int currentFrame)
{
    _previousFrame = _currentFrame;
    _currentFrame = currentFrame;
}

unsigned int ScenePlayer::getNextFrame()
{
    _previousFrame = _currentFrame;

    if (_currentFrame < _lastFrame)
        _currentFrame++;
    else
        _currentFrame = _firstFrame;

    return _currentFrame;
}

bool ScenePlayer::isReady()
{
    return (_frameCount > 0);
}

bool ScenePlayer::isFirstFrame()
{
    return (_currentFrame == _firstFrame);
}

bool ScenePlayer::isLastFrame()
{
    return (_currentFrame == _lastFrame);
}

bool ScenePlayer::frameHasChanged()
{
    return (_previousFrame != _currentFrame);
}

unsigned int ScenePlayer::getFirstFrame()
{
    return _firstFrame;
}
