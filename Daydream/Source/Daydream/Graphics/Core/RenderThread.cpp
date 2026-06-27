#include "DaydreamPCH.h"
#include "RenderThread.h"

#include "RenderCommandQueue.h"

namespace Daydream
{
	RenderThread::RenderThread()
	{
	}

	RenderThread::~RenderThread()
	{
	}

	void RenderThread::Start()
	{
		//1로 덮어쓰기 전의 값이 1이었으면 함수 종료
		if (isRunning.exchange(1))
		{
			return;
		}

		//스레드 실행 jthread의 stoptoken 넘겨서 안전한 종료 대비 및 워커 스레드의 루프 실행
		workerThread = std::jthread([this](std::stop_token _stopToken)
			{
				WorkerLoop(_stopToken);
			});
	}
	void RenderThread::Stop()
	{
		//0으로 덮어쓰기 전의 값이 0 이었으면 함수 종료
		if (!isRunning.exchange(0))
		{
			return;
		}

		if (workerThread.joinable())
		{
			workerThread.request_stop();
			queueCV.notify_all();
			workerThread.join();
		}
	}
	void RenderThread::Submit(RenderCommandQueue* _queue, FunctionPtr<void()> _onComplete)
	{
		DAYDREAM_CORE_ASSERT(_queue, "RenderThread::Submit - queue is null!");
		DAYDREAM_CORE_ASSERT(isRunning, "RenderThread::Submit - render thread is not running!");

		{
			std::lock_guard<std::mutex> lock(queueMutex);
			submitQueue.push({ _queue, _onComplete });
			++inFlightCount;
		}

		queueCV.notify_one();
	}
	void RenderThread::WaitIdle()
	{
		std::unique_lock<std::mutex> lock(idleMutex);
		idleCV.wait(lock, [this]()
			{
				return inFlightCount.load() == 0;
			});
	}

	void RenderThread::WorkerLoop(std::stop_token _stopToken)
	{
		while(true)
		{
			SubmittedQueue queueToExecute;
			{
				// 큐에서 일감을 꺼내기 위해 락을 검
				// 이 작업중에는 메인스레드에서 렌더스레드에 큐를 제출할 수 없도록
				std::unique_lock<std::mutex> lock(queueMutex);

				// wait의 조건을 확인하고 조건이 true라면 그냥 넘어감
				// 조건이 아무것도맞지 않으면 notify를 기다리는 상태가 됨
				// 이후 notify가 들어오면 다음 3가지 조건 중 하나라도 만족하면 락을 다시 가지고 넘어감
				// 1) 큐에 일감이 들어왔을 때
				// 2) 스레드 종료 요청이 들어왔을 때
				// 3) isRunning이 false로 바뀌었을 때
				queueCV.wait(lock, [this, _stopToken]()
					{
						return !submitQueue.empty() || _stopToken.stop_requested() || !isRunning.load();
					});

				// 깨어났는데 큐가 비어있다면, 이것은 일감 때문이 아니라 종료 요청
				if (submitQueue.empty())
				{
					if (_stopToken.stop_requested() || !isRunning.load())
					{
						break; // 무한 루프를 부수고 스레드를 완전히 종료
					}
					continue; // 만약의 가짜 깨어남이라면 다시 루프의 처음으로
				}

				queueToExecute = submitQueue.front();
				submitQueue.pop();
			}//mutex 해제

			if (queueToExecute.queue)
			{
				//렌더링 실행
				queueToExecute.queue->Execute();

				// 콜백함수 실행
				if (queueToExecute.onComplete)
				{
					queueToExecute.onComplete();
				}

				// 카운터 감소
				--inFlightCount;

				// 메인스레드한테 일끝났다고 알리기
				idleCV.notify_all();
			}
		}
	}
}

