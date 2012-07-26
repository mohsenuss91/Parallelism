#include "pipeline.hpp"
#include "img_processing.hpp"
#include "videoProcessing.hpp"


Chunk::Chunk(cv::Mat* offset1, unsigned size, cv::Mat* offset2)
    : offset1_(offset1),
      offset2_(offset2),
      size_(size)
{}

OutputVideo::OutputVideo(cv::VideoCapture& vid)
    : vid("resources/res.avi",
          vid.get(CV_CAP_PROP_FOURCC),
          vid.get(CV_CAP_PROP_FPS),
          cv::Size(vid.get(CV_CAP_PROP_FRAME_WIDTH),
                   vid.get(CV_CAP_PROP_FRAME_HEIGHT)),
          true)
{}

void
OutputVideo::operator()(Chunk* c) const
{
    auto v = c->getFrames().first;
    unsigned size = c->getFrames().second;

    for (unsigned i = 0; i < size; ++i)
        vid << v[i];

    delete c;
}

InputVideo::InputVideo(const std::vector<cv::Mat>& vid,
                       const std::vector<cv::Mat>& vid2)
    : vid_(vid),
      vid2_(vid2),
      offset(0)
{}

Chunk*
InputVideo::operator()(tbb::flow_control& fc) const
{
    unsigned size = 0;
    cv::Mat* v2 = nullptr;

    if (vid_.end() - (vid_.begin() + offset) >= Chunk::chunkSize)
        size = Chunk::chunkSize;
    else {
        size = vid_.end() - (vid_.begin() + offset);
        fc.stop();
    }

    if (!vid2_.empty())
        v2 = vid2_.data() + offset;

    Chunk* c = new Chunk(vid_.data() + offset, size, v2);

    offset += size;

    return c;
}

Chunk*
Transformer::operator()(Chunk* c) const
{
    auto v = c->getFrames().first;
    unsigned size = c->getFrames().second;
    auto v2 = c->getFrames2().first;

    if (v2) {
        for (unsigned i = 0; i < size; ++i)
            v[i] = proc::blur(v[i]);
    }
    else {
        for (unsigned i = 0; i < size; ++i)
            v[i] = proc::blur(v[i]);
    }

    return c;
}