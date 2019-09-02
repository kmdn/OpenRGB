#include "AuraController.h"
#include "RGBController.h"
#include "RGBController_Aura.h"
#include "i2c_smbus.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>

/******************************************************************************************\
*                                                                                          *
*   TestForAuraController                                                                  *
*                                                                                          *
*       Tests the given address to see if an Aura controller exists there.  First does a   *
*       quick write to test for a response, and if so does a simple read at 0xA0 to test   *
*       for incrementing values 0...F which was observed at this location during data dump *
*                                                                                          *
\******************************************************************************************/

bool TestForAuraController(i2c_smbus_interface* bus, unsigned char address)
{
    bool pass = false;

    int res = bus->i2c_smbus_write_quick(address, I2C_SMBUS_WRITE);

    if (res >= 0)
    {
        pass = true;

        for (int i = 0xA0; i < 0xB0; i++)
        {
            res = bus->i2c_smbus_read_byte_data(address, i);

            if (res != (i - 0xA0))
            {
                pass = false;
            }
        }
    }

    return(pass);

}   /* TestForAuraController() */

/******************************************************************************************\
*                                                                                          *
*   DetectAuraControllers                                                                  *
*                                                                                          *
*       Detect Aura controllers on the enumerated I2C busses.  Searches for Aura-enabled   *
*       RAM at 0x77 and tries to initialize their slot addresses, then searches for them   *
*       at their correct initialized addresses.  Also looks for motherboard controller at  *
*       address 0x4E.                                                                      *
*                                                                                          *
*           bus - pointer to i2c_smbus_interface where Aura device is connected            *
*           dev - I2C address of Aura device                                               *
*                                                                                          *
\******************************************************************************************/

void DetectAuraControllers(std::vector<i2c_smbus_interface*> &busses, std::vector<RGBController*> &rgb_controllers)
{
    AuraController* new_aura;
    RGBController_Aura* new_controller;

    for (unsigned int bus = 0; bus < busses.size(); bus++)
    {
        // Remap Aura-enabled RAM modules on 0x77
        for (unsigned int slot = 0; slot < 8; slot++)
        {
            int res = busses[bus]->i2c_smbus_write_quick(0x77, I2C_SMBUS_WRITE);

            if (res < 0)
            {
                break;
            }

            AuraController temp_controller(busses[bus], 0x77);

            temp_controller.AuraRegisterWrite(AURA_REG_SLOT_INDEX, slot);
            temp_controller.AuraRegisterWrite(AURA_REG_I2C_ADDRESS, 0xE0 + (slot << 1));
        }

        // Add Aura-enabled controllers at their remapped addresses
        for (unsigned int slot = 0; slot < 8; slot++)
        {
            if (TestForAuraController(busses[bus], 0x70 + slot))
            {
                new_aura = new AuraController(busses[bus], 0x70 + slot);
                new_controller = new RGBController_Aura(new_aura);
                rgb_controllers.push_back(new_controller);
            }
        }

        // Check for Aura controller at 0x40
        if (TestForAuraController(busses[bus], 0x40))
        {
            new_aura = new AuraController(busses[bus], 0x40);
            new_controller = new RGBController_Aura(new_aura);
            rgb_controllers.push_back(new_controller);
        }

        // Check for Aura controller at 0x4E
        if (TestForAuraController(busses[bus], 0x4E))
        {
            new_aura = new AuraController(busses[bus], 0x4E);
            new_controller = new RGBController_Aura(new_aura);
            rgb_controllers.push_back(new_controller);
        }

        // Check for Aura controller at 0x4F
        if (TestForAuraController(busses[bus], 0x4F))
        {
            new_aura = new AuraController(busses[bus], 0x4F);
            new_controller = new RGBController_Aura(new_aura);
            rgb_controllers.push_back(new_controller);
        }

        // Check for Aura controller at 0x66
        if (TestForAuraController(busses[bus], 0x66))
        {
            new_aura = new AuraController(busses[bus], 0x66);
            new_controller = new RGBController_Aura(new_aura);
            rgb_controllers.push_back(new_controller);
        }
    }

}   /* DetectAuraControllers() */