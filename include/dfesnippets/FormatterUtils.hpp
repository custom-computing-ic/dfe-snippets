#ifndef FORMATTERUTILS_HPP_U2I01W8O
#define FORMATTERUTILS_HPP_U2I01W8O

#include <chrono>
#include <iostream>
#include <dfesnippets/timing/Timing.hpp>

namespace dfesnippets {
  namespace formatting {

    class ResultsFormatter {
      long flops, dataSize;

      double dramWriteTime = -1;
      double dramReadTime = -1;
      std::chrono::high_resolution_clock::time_point firstStart;
      std::chrono::high_resolution_clock::time_point start;

      bool timing = false;
      bool first = true;

      long dramReadSize, dramWriteSize;

      double computeTime;

      public:
      ResultsFormatter(
          long _flops,
          long _dataSize) : flops(_flops), dataSize(_dataSize) {
      }

      double toGBps(double bytes, double seconds) {
        return toGB(bytes) / seconds;
      }

      double toGB(double bytes) {
        return bytes / (1024.0 * 1024 * 1024);
      }

      double toGFlops(double flops) {
        return flops / 1E9;
      }

      void setDramWrite(double t, long sizeBytes) {
        this->dramWriteTime = t;
        this->dramWriteSize = sizeBytes;
      }

      void setDramRead(double t, long sizeBytes) {
        this->dramReadTime = t;
        this->dramReadSize = sizeBytes;
      }

      void setComputeTime(double t) {
        this->computeTime = t;
      }

      void print() {
        double gflops = toGFlops(flops);
        double totalRunTime = dfesnippets::timing::clock_diff(firstStart);

        if (dramReadTime != -1) {
          std::cout << "Dram Read" << std::endl;
          std::cout << "  Time (s)           = " << dramReadTime << std::endl;
          std::cout << "  Size (GB)          = " << toGB(dramReadSize) << std::endl;
          std::cout << "  Bwidth (GB/s)      = " << toGBps(dramReadSize, dramReadTime) << std::endl;
        }

        if (dramWriteTime != -1) {
          std::cout << "Dram Write " << std::endl;
          std::cout << "  Time (s)           = " << dramWriteTime << std::endl;
          std::cout << "  Size (GB)          = " << toGB(dramWriteSize) << std::endl;
          std::cout << "  Bwidth (GB/s)      = " << toGBps(dramWriteSize, dramReadTime) << std::endl;
        }

        std::cout << "Compute " << std::endl;
        std::cout <<   "  Time               = " << computeTime << std::endl;
        std::cout <<   "  GFLOPS             = " << gflops << std::endl;
        std::cout <<   "  GFLOPS/s           = " << gflops / computeTime << std::endl;
        std::cout <<   "  Bwidth (GB/s)      = " << toGBps(dataSize, computeTime) << std::endl;

        std::cout <<   "Running Total (s)    = " << totalRunTime << std::endl;
        std::cout <<   "Total GFLOPS         = " << gflops / totalRunTime << std::endl;
      }

      void startTiming() {
        if (timing) {
          std::cerr << "Timing already started, this is probably an error" << std::endl;
        }
        if (first) {
          first = false;
          firstStart = std::chrono::high_resolution_clock::now();
        }
        start = std::chrono::high_resolution_clock::now();
        timing = true;
      }

      // stops the timer and returns time elapsed (in ms) since the most recent startTiming call
      double stopTiming() {
        double diff = dfesnippets::timing::clock_diff(start);
        if (!timing) {
          std::cerr << "Timing not started, this is probably an error" << std::endl;
        }
        timing = false;
        return diff;
      }

    };

  }
}

#endif /* end of include guard: FORMATTERUTILS_HPP_U2I01W8O */
