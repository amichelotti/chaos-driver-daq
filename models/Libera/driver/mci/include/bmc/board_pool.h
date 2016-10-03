/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: board_pool.h 14606 2011-12-21 12:43:41Z matej.kenda $
 */

#ifndef BMC_BOARD_POOL_H
#define BMC_BOARD_POOL_H

#include <map>
#include <thread>

#include "bmc/uri.h"
#include "bmc/types.h"

namespace bmc {

    class Board;
    typedef std::map<Uri, Board*> BoardMap;

	/**
	 * @addtogroup bmc_board Boards handling Classes and functions
	 * @{
	 */
    class BoardPool {
        public:
            /**
             * By default the BoardPoll connects to Platform B chassis which has the ICB
             * boards which handle all the other boards.
             */
            static BoardPool& Instance();
            Board* GetBoard(const Uri& a_id);
            UriVector GetAllLocalBoardUris() const;
            // GetAllBoards?
        private:
            /**
             *  singleton private constructor
             */
            BoardPool();
            ~BoardPool();

            /**
             * Reads all the information about the boards again.
             * Called in constructor.
             */
            void Update();
            void UpdateBoards();

            // copying not allowed
            BoardPool(const BoardPool& other);
            BoardPool& operator=(const BoardPool& other);

            BoardMap            m_boards;

            std::mutex          m_boards_x;
    };

    inline Board* GetBoard(const Uri& a_id)
    {
        return BoardPool::Instance().GetBoard(a_id);
    }

    /**
     * Platform B uses this method to create a BoardPool. For back-compatibility the chassis type
     * is set by default
     */
    inline UriVector GetAllLocalBoardUris()
    {
        return BoardPool::Instance().GetAllLocalBoardUris();
    }

    std::string GetDevicePrefix(const Uri& a_id);

    /**@}*/
}

#endif /* BMC_BOARD_POOL_H */
