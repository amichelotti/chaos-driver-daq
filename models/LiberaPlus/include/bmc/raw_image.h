/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: raw_image.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef BMC_RAW_IMAGE_H
#define BMC_RAW_IMAGE_H

#include <vector>

namespace bmc {

    typedef std::vector<unsigned char>   ImageData;

	/**
	 * @addtogroup bmc_image
	 * @{
	 */
    /** Container for image data, containing for example FPGA bitstream. */
    class RawImage {
        public:
            /** Construct the image from ImageData (copies a_rawImageData) */
            explicit RawImage(const ImageData& a_rawImageData);

            /** Construct the image from pointer to a buffer (copies a_buffer) */
            explicit RawImage(const unsigned char* a_buffer, size_t a_size);

            ~RawImage();

            const ImageData& GetBuffer() const;

            /**
             * Attempts to remove headers from FPGA bitstream before
             * Xilinx or Lattice sync word.
             */
            bool StripToSyncWord();

        private:
            ImageData   m_rawImageData;
    };
    /**@}*/

    inline bool operator==(const bmc::RawImage& a_lhs, const bmc::RawImage& a_rhs)
    {
        // Compare buffers (vectors).
        return (a_lhs.GetBuffer() == a_rhs.GetBuffer());
    }

}

#endif /* BMC_RAW_IMAGE_H */
