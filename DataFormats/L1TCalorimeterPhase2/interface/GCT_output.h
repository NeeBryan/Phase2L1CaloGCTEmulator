#ifndef DataFormats_L1TCalorimeterPhase2_GCT_output_h
#define DataFormats_L1TCalorimeterPhase2_GCT_output_h

#include <ap_int.h>
#include <vector>

namespace l1tp2 {

  class gctOutputLink {
  public:
    gctOutputLink() : linkData((ap_uint<576>)0) {}
    explicit gctOutputLink(ap_uint<576> data) : linkData(data) {}

    ap_uint<576> data() const { return linkData; }

  private:
    ap_uint<576> linkData;
  };

  typedef std::vector<l1tp2::gctOutputLink> gctOutputLinkCollection;

}  // namespace l1tp2

#endif