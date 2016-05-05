/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: uri.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef BMC_URI_H
#define BMC_URI_H

#include <string>
#include <vector>
#include <mutex>

namespace bmc {

	/**
	 * @addtogroup bmc_board
	 * @{
	 */

    /**
     * Uniform Resource Identifier for objects accessed via libbmc.
     *
     * Generic format of URI is: <scheme>:<hierarchical part>
     *
     * For the purpose of the libbmc, the following full syntax is used:
     *
     * - lbr://<location>/<subsystem>[;interface]/<id>
     *
     * Examples:
     * - Full URI and abbreviations for ICB
     *   - lbr://local/chassis;ipmi-usb/0
     *   - lbr://local/chassis/0
     *   - lbr:///chassis/0
     *   - chassis/0
     *   - 0
     * - OS "board"
     *   - lbr://local/os
     *   - os
     * - uTCA MCH
     *   - lbr://local/mch;hpi
     *   - lbr:///mch
     *   - mch
     * - uTCA AMC boards
     *   - lbr://local/amc;hpi/1
     *   - lbr:///amc/1
     *   - amc/1
     *
     * Since local URIs are used most commonly, the following
     * abbreviations are possible:
     *
     * - "lbr:///1", "chassis/1", and "1" are equal to "lbr://local/amc/1"
     * - "os" is equal to lbr://local/os
     *
     * See details about URIs in general on wikipedia:
     * - http://en.wikipedia.org/wiki/Uniform_Resource_Identifier
     * - http://en.wikipedia.org/wiki/URI_scheme
     */
    class Uri {
        public:
            static const std::string c_schema;

            static const std::string c_locLocal;
            enum SubSystem_e {
                eSChassis,          // Architecture B chassis
                eSAmc,              // AMC device
                eSRtm,              // Rear transition module
                eSCoolingUnit,      // Cooling unit
                eSPowerSupply,      // Power supply
                eSUtcaCarrierHub,   // uTCA MCH
                eSOperatingSystem,  // Operating system
                eSTest,
                eSUnknown
            };
            enum Interface_e {
                eIfDefault,
                eIfIpmiUsb,
                eIfIpmiI2c,
                eIfHpi,
                eIfIpmiLan
            };

            Uri();
            explicit Uri(
                const std::string& a_location,
                SubSystem_e a_subsys,
                const std::string& a_id,
                Interface_e a_interface = eIfDefault);

            /** Ctor for local URIs */
            explicit Uri(
                SubSystem_e a_subsys,
                const std::string& a_id = "",
                Interface_e a_interface = eIfDefault);

            /** Ctor for local URIs with numerical IDs */
            explicit Uri(
                SubSystem_e a_subsys,
                int a_id,
                Interface_e a_interface = eIfDefault);

            ~Uri();

            const std::string&      GetLocation() const;
            bmc::Uri::SubSystem_e   GetSubSystem() const;
            std::string             GetSubSystemStr() const;
            bmc::Uri::Interface_e   GetInterface() const;
            const std::string&      GetId() const;

            void SetLocation(const std::string& a_location);
            void SetSubSystem(bmc::Uri::SubSystem_e a_subsys);
            void SetInterface(bmc::Uri::Interface_e a_iface);
            void SetId(const std::string& a_id);

            bool IsLocal() const;

            const std::string&  ToString() const;

            Uri(const Uri& a_uri);
            Uri& operator = (const Uri & a_other);

        private:
            mutable std::string     m_qualified;
            mutable bool            m_dirty;
            mutable std::mutex      m_data_x;

            std::string         m_location;
            SubSystem_e         m_subsys;
            Interface_e         m_interface;
            std::string         m_id;

            void UpdateQualified() const;
    };
    /**@}*/

    inline bool operator<(const Uri& a_lhs, const Uri& a_rhs)
    {
        return a_lhs.ToString() < a_rhs.ToString();
    }

    inline bool operator==(const bmc::Uri& a_lhs, const bmc::Uri& a_rhs)
    {
        return (a_lhs.ToString() == a_rhs.ToString());
    }

    typedef std::vector<Uri>    UriVector;

	/**
	 * @addtogroup bmc_board
	 * @{
	 */
    /** Parses and decomposes the stringified URI to create an Uri. */
    Uri DecomposeUri(const std::string& a_uri);

    /** Helper function to create a valid Uri for local boards in the chassis.
     * lbr:///amc/<a_boardNum>
     */
    Uri CreateLocalChassisUri(int a_boardNum, Uri::Interface_e a_uri = Uri::eIfDefault);

    /** Helper function to create a valid Uri for local boards in the chassis.
     * lbr:///amc/<a_boardNum>
     */
    Uri CreateLocalAmcUri(int a_boardNum, Uri::Interface_e a_uri = Uri::eIfDefault);

    /**@}*/

    extern const Uri c_localIcbUri;
    extern const Uri c_localOSUri;

}

#endif /* BMC_URI_H */
