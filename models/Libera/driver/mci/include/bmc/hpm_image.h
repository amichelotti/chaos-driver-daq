/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: hpm_image.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef HPM_IMAGE_H
#define HPM_IMAGE_H

#include <string>
#include <stdint.h>
#include "bmc/raw_image.h"
#include <bmc/types.h>

namespace bmc {

    const int c_maxHpmMajorVersion = 0x7F; // 7 bits are reserved for major version
    const int c_maxHpmMinorVersion = 99;   // 1 byte in BCD encoding is reserved for minor version
    const unsigned int c_maxHpmDescription = 21;    // 21 bytes is reserverd for description
    const unsigned int c_maxHpmBoardType   = 5;     // 5 bytes reserved for board type

	/**
	 * @addtogroup bmc_image FPGA image handling Classes and functions
	 * @{
	 */

    /** Container for HPM image data, containing e.g. FPGA bitstream */
    class HpmImage {
        public:
            /** Construct the image from RawImage. Copies a_rawImage and
             * equips it with IPMI HPM headers and MD5 checksum.
             */
            explicit HpmImage(const RawImage& a_rawImage, uint8_t a_majVer, uint8_t a_minVer,
                              uint32_t a_svnRev, bmc::BoardType_e a_boardType);

            /** Buffer containing HPM-compliant firmware image */
            const ImageData& GetBuffer() const;

            /** Returns version of HPM image */
            void GetHpmImageVersion(uint8_t &a_majVer, uint8_t &a_minVer, uint32_t &a_svnRev) const;

            /** Returns type of HPM image - for which board */
            bmc::BoardType_e GetHpmImageType() const;

            /** Convert HpmImage to RawImage */
            operator RawImage() const;

            ~HpmImage();

        private:
            ImageData   m_hpmImageData;

            /** Construct the image from ImageData (copies a_rawImageData).
             * @param a_hpmImageData Valid HPM data buffer
             */
            explicit HpmImage(const ImageData& a_hpmImageData);

            /** Construct the image from pointer to a buffer (copies a_buffer)
             * @param a_buffer Pointer to a valid HPM data buffer
             */
            explicit HpmImage(const unsigned char* a_buffer, size_t a_size);

            friend HpmImage CreateHpmImage(const ImageData& a_hpmImageData);
            friend HpmImage CreateHpmImage(const unsigned char* a_buffer, size_t a_size);
    };

    inline bool operator==(const bmc::HpmImage& a_lhs, const bmc::HpmImage& a_rhs)
    {
        // Compare buffers (vectors).
        return (a_lhs.GetBuffer() == a_rhs.GetBuffer());
    }

    inline bool operator!=(const bmc::HpmImage& a_lhs, const bmc::HpmImage& a_rhs)
    {
        return !( a_lhs == a_rhs );
    }

    /** Validates the buffer and creates new HpmImage.
     * @throw istd::Exception
     */
    HpmImage CreateHpmImage(const ImageData& a_hpmImageData);

    /** Validates the buffer and creates new HpmImage.
     * @throw istd::Exception
     */
    HpmImage CreateHpmImage(const unsigned char* a_buffer, size_t a_size);

    /** Read FPGA binary image from file and return a ImageDate buffer
     * @throw istd::Exception
     */
    ImageData ReadImageFromFile(const std::string& a_fileName);

    /** Write ImageData to file
     * @throw istd::Exception
     */
    void WriteImageToFile(const std::string& a_fileName, const ImageData& a_image);

    /**@}*/

}
#endif /* HPM_IMAGE_H */
