#if defined(__unix__) || defined(__APPLE__)

// sys
#include <errno.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

// sfc
#include "../../fs.h"
#include "../../io.h"

namespace sfc::io {

template <usize FD>
struct RawStdIO : fs::File {
  RawStdIO() : fs::File{FD} {}

  ~RawStdIO() {
    mem::forget<fs::File>(*this);
  }

  static auto winsize() -> WinSize {
    struct winsize ws;
    auto ret = ::ioctl(FD, TIOCGWINSZ, &ws);
    if (ret == -1 || ws.ws_col == 0) {
      return WinSize{.cols = 24, .rows = 80};
    }
    return WinSize{.cols = usize(ws.ws_col), .rows = usize(ws.ws_row)};
  }
};

struct RawStdin {
  static constexpr int FILE_NO = STDIN_FILENO;
  static auto read(Slice<u8> buf) -> usize;
  static auto winsize() -> WinSize;
};

struct RawStdout {
  static constexpr int FILE_NO = STDOUT_FILENO;

  static auto write(Slice<const u8> buf) -> usize;
  static auto winsize() -> WinSize;
};

struct RawStderr {
  static constexpr int FILE_NO = STDERR_FILENO;
  static auto write(Slice<const u8> buf) -> usize;
  static auto winsize() -> WinSize;
};

auto RawStdin::read(Slice<u8> buf) -> usize {
  static auto imp = RawStdIO<FILE_NO>{};
  return imp.read(buf);
}

auto RawStdout::write(Slice<const u8> buf) -> usize {
  static auto imp = RawStdIO<FILE_NO>{};
  return imp.write(buf);
}

auto RawStdout::winsize() -> WinSize {
  return RawStdIO<FILE_NO>::winsize();
}

auto RawStderr::write(Slice<const u8> buf) -> usize {
  static auto imp = RawStdIO<FILE_NO>{};
  return imp.write(buf);
}

auto RawStderr::winsize() -> WinSize {
  return RawStdIO<FILE_NO>::winsize();
}

auto Error::last_os_error() -> Error {
  return {errno};
}

auto Error::kind() const -> Kind {
  switch (code) {
    case EPERM:
      return Error::PermissionDenied;
    case ENOENT:
      return Error::NotFound;
    case EINTR:
      return Error::Interrupted;
    case EWOULDBLOCK:  // EAGAIN
      return Error::WouldBlock;
    case EACCES:
      return Error::PermissionDenied;
    case EEXIST:
      return Error::AlreadyExists;
    case EINVAL:
      return Error::InvalidInput;
    case EPIPE:
      return Error::BrokenPipe;
    case EADDRNOTAVAIL:
      return Error::AddrNotAvailable;
    case ENOTCONN:
      return Error::NotConnected;
    case ECONNABORTED:
      return Error::ConnectionAborted;
    case ECONNRESET:
      return Error::ConnectionReset;
    case EADDRINUSE:
      return Error::AddrInUse;
    case ETIMEDOUT:
      return Error::TimedOut;
    case ECONNREFUSED:
      return Error::ConnectionRefused;
    default:
      break;
  };
  return Error::Other;
}

}  // namespace sfc::io

#endif
