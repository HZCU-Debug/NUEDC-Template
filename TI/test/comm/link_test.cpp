#include <cassert>
#include <cstdint>
#include <vector>

#include "comm/link.h"
#include "support/fakes.h"

int main() {
    FakeStream stream;
    FakeClock clock;
    comm::Link<8> link(stream, clock);

    assert(link.begin());
    assert(link.send(0, nullptr, 0, comm::Delivery::Unreliable) ==
           comm::SendResult::InvalidArgument);
    assert(link.send(1, nullptr, 1, comm::Delivery::Unreliable) ==
           comm::SendResult::InvalidArgument);
    const uint8_t oversized[9] = {};
    assert(link.send(1, oversized, sizeof(oversized),
                     comm::Delivery::Unreliable) ==
           comm::SendResult::PayloadTooLarge);

    const uint8_t payload[] = {0x11, 0x00, 0x22};
    assert(link.send(1, payload, sizeof(payload),
                     comm::Delivery::Unreliable) ==
           comm::SendResult::Accepted);
    assert(stream.output ==
           std::vector<uint8_t>({0x03, 0x01, 0x11, 0x03, 0x22, 0x31, 0x00}));

    stream.output.clear();
    const uint8_t reliablePayload[] = {0x33};
    assert(link.send(1, reliablePayload, sizeof(reliablePayload),
                     comm::Delivery::Reliable) ==
           comm::SendResult::Accepted);
    const std::vector<uint8_t> firstAttempt = stream.output;
    assert(firstAttempt ==
           std::vector<uint8_t>({0x02, 0x81, 0x03, 0x33, 0xF9, 0x00}));
    assert(link.send(2, nullptr, 0, comm::Delivery::Unreliable) ==
           comm::SendResult::Busy);

    clock.advance(49);
    assert(link.poll().type == comm::EventType::None);
    assert(stream.output == firstAttempt);
    clock.advance(1);
    assert(link.poll().type == comm::EventType::None);
    std::vector<uint8_t> repeated = firstAttempt;
    repeated.insert(repeated.end(), firstAttempt.begin(), firstAttempt.end());
    assert(stream.output == repeated);

    stream.receive({0x01, 0x01, 0x01, 0x01, 0x00});
    assert(link.poll().type == comm::EventType::Delivered);
    assert(link.send(2, nullptr, 0, comm::Delivery::Unreliable) ==
           comm::SendResult::Accepted);

    stream.receive({0x03, 0x01, 0x11, 0x03, 0x22, 0x31, 0x00});
    const comm::Event unreliable = link.poll();
    assert(unreliable.type == comm::EventType::Message);
    assert(unreliable.message.type == 1);
    assert(unreliable.message.delivery == comm::Delivery::Unreliable);
    assert(unreliable.message.size == 3);
    assert(unreliable.message.payload[0] == 0x11);
    assert(unreliable.message.payload[1] == 0x00);
    assert(unreliable.message.payload[2] == 0x22);

    stream.output.clear();
    stream.receive({0x05, 0x81, 0x07, 0x44, 0xD0, 0x00});
    const comm::Event reliable = link.poll();
    assert(reliable.type == comm::EventType::Message);
    assert(reliable.message.delivery == comm::Delivery::Reliable);
    assert(reliable.message.payload[0] == 0x44);
    assert(stream.output ==
           std::vector<uint8_t>({0x01, 0x03, 0x07, 0x15, 0x00}));

    stream.receive({0x05, 0x81, 0x07, 0x44, 0xD0, 0x00});
    assert(link.poll().type == comm::EventType::None);
    assert(stream.output.size() == 10);

    FakeStream recoveryStream;
    FakeClock recoveryClock;
    comm::Link<8> recoveryLink(recoveryStream, recoveryClock);
    assert(recoveryLink.begin());
    recoveryStream.receive({0x03, 0x01, 0x11, 0x03, 0x22, 0x30, 0x00,
                            0x05, 0x01, 0x00,
                            0x03, 0x01, 0x11, 0x03, 0x22, 0x31, 0x00});
    assert(recoveryLink.poll().type == comm::EventType::Message);

    FakeStream rolloverStream;
    FakeClock rolloverClock;
    rolloverClock.now = UINT32_MAX - 20;
    comm::Link<1> rolloverLink(rolloverStream, rolloverClock,
                               comm::LinkConfig(50));
    assert(rolloverLink.begin());
    assert(rolloverLink.send(1, nullptr, 0, comm::Delivery::Reliable) ==
           comm::SendResult::Accepted);
    rolloverClock.advance(50);
    assert(rolloverLink.poll().type == comm::EventType::None);
    assert(rolloverStream.output.size() == 10);

    FakeStream failedStream;
    FakeClock failedClock;
    comm::Link<1> failedLink(failedStream, failedClock);
    assert(failedLink.begin());
    failedStream.failWrite = true;
    assert(failedLink.send(1, nullptr, 0, comm::Delivery::Unreliable) ==
           comm::SendResult::WriteFailed);
}
