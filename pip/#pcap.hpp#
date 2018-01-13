#pragma once

#include <cstdint>
#include <stdexcept>

#include <pcap/pcap.h>

#include <sys/time.h>

namespace pip
{
namespace cap
{
  /// A packet provides a view into captured data from the device.
  class packet
  {
    friend class file;
  public:
    packet()
      : hdr(nullptr), buf(nullptr)
    { }

    // Returns the number of bytes actually captured. The captured
    // length is less than or equal to total size.
    int size() const { return hdr->caplen; }

    // Returns the total number of bytes in the packet. Note that
    // this may be larger than the number of bytes captured.
    int total_size() const { return hdr->len; }

    // Returns true when the packet is fully captured.
    bool is_complete() const { return size() == total_size(); }

    /// Returns the time stamp of the packet. This is time elapsed since 
    /// the beginning of the capture (not the absolute time of the capture).
    ///
    /// \todo Convert this to a chrono value.
    timeval timestamp() const { return hdr->ts; }

    // Returns the underlying packet data.
    const unsigned char* data() const { return buf; }

  private:
    pcap_pkthdr* hdr;
    const unsigned char* buf;
  };


  /// Provides access to a captured stream of packets.
  ///
  /// \todo Provide a facility for returning the link type for the packet.
  /// This can be used to establish the lowest-level protocol layer.
  struct file
  {
  public:
    file(const char* path);
    ~file();

    /// Returns true if the file is open and not in error.
    bool ok() const { return status > 0; }

    /// Converts to true if the file is ok.
    explicit operator bool() const { return ok(); }

    /// Attempt to get the next packet from the stream. Returns his object.
    /// If, after calling this function, the stream is not in a good state,
    /// the packet `p` is partially formed.
    file& get(packet& p);

  private:
    /// An error message (256 bytes)
    char error[PCAP_ERRBUF_SIZE];

    /// The underlying device
    pcap_t* handle;
    
    /// Result of the last get.
    int status;
  };

  inline
  file::file(const char* path)
    : handle(pcap_open_offline(path, error))
  {
    if (!handle)
      throw std::runtime_error(error);
  }

  inline
  file::~file()
  {
    pcap_close(handle);
  }

  inline file&
  file::get(packet& p)
  {
    do {
      status = ::pcap_next_ex(handle, &p.hdr, &p.buf);
    } while (status == 0);
    return *this;
  }

} // namespace cap
} // namespace pip

