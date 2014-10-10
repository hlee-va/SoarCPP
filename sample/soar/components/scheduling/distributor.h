/*
 * distributor.h
 *
 *  Created on: Feb 27, 2013
 *      Author: House_Lee
 */

#ifndef SOAR_COMPONENTS_SCHEDULING_DISTRIBUTOR_H_
#define SOAR_COMPONENTS_SCHEDULING_DISTRIBUTOR_H_

#include "../network/socket.h"
#include "../../communal.h"
#include "pipeline.h"

namespace soar_components_scheduling {

class Distributor_Interface {
public:
	virtual SoarDataBlock RecvMessage(const soar_components_network::SoarSocket& socket) = 0;
	virtual PipeLineIndex GetMsgDestination() = 0;
	virtual ~Distributor_Interface(){};
};

}/*namespace soar_components_scheduling*/



#endif /* SOAR_COMPONENTS_SCHEDULING_DISTRIBUTOR_H_ */
