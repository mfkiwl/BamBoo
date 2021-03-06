#include "Vram.h" // Change this
#include "verilated.h"
#include "../include/sim_common.h"
#include <iostream>

#define CLK_PERIOD 10

int main(int argc, char** argv, char** env) {
    Verilated::commandArgs(argc, argv);

    Vram* top = new Vram; // Change this

    SIM_INIT;

    SIM_START(100)

        CLOCK(top->clk, CLK_PERIOD);
        
        // Write byte (0x42) to address 3
        SET_SIG(8, top->en, 1);
        SET_SIG(8, top->data_i, 0x42);
        SET_SIG(8, top->mem_we, 1);
        SET_SIG(8, top->byte_sel, 0);
        SET_SIG(8, top->addr_a, 3);
        SET_SIG(13, top->mem_we, 0);
        SET_SIG(13, top->en, 0);
        
        // Read from address 3 on port a --> @25: data_a = 0x00000042
        SET_SIG(21, top->en, 1);
        SET_SIG(21, top->addr_a, 3);
        SET_SIG(23, top->en, 0);

        // Write half-word (0xDEAD) to address 0
        SET_SIG(29, top->en, 1);
        SET_SIG(29, top->data_i, 0xDEAD);
        SET_SIG(29, top->mem_we, 1);
        SET_SIG(29, top->byte_sel, 1);
        SET_SIG(29, top->addr_a, 0);
        SET_SIG(34, top->mem_we, 0);
        SET_SIG(34, top->en, 0);

        // Read from address 0 on port a --> @36: data_a = 0x4200DEAD
        SET_SIG(36, top->en, 1);
        SET_SIG(36, top->addr_a, 0);
        SET_SIG(37, top->en, 0);

        // Write word (0xAFFE1234) to address 4
        SET_SIG(39, top->en, 1);
        SET_SIG(39, top->data_i, 0xAFFE1234);
        SET_SIG(39, top->mem_we, 1);
        SET_SIG(39, top->byte_sel, 2);
        SET_SIG(39, top->addr_a, 4); // @39: data_a = 0
        SET_SIG(44, top->mem_we, 0); // @40: data_a = 0xAFFE1234
        SET_SIG(44, top->en, 0);

        // Read from address 4 on port b --> @52: data_b = 0xAFFE1234
        SET_SIG(52, top->addr_b, 4);

        // Read from address 2 on part a --> @70: data_a = 0x12344200
        SET_SIG(70, top->en, 1);
        SET_SIG(70, top->addr_a, 2);
        SET_SIG(71, top->en, 0);

        // Read from address 32 on port a, but no "en" --> @80: data_a = 0x12344200
        SET_SIG(80, top->addr_a, 32);

        // Read from address 32 on port a, but with "en" --> @90: data_a = 0
        SET_SIG(90, top->en, 1);
        SET_SIG(90, top->addr_a, 32);
        SET_SIG(91, top->en, 0);

        top->eval();

        /*if((time % CLK_PERIOD) == 0 || (time % CLK_PERIOD) == CLK_PERIOD/2)*/ {   // Read outputs
            std::cout << ">>> Time = " << time << std::endl;
            std::cout << "data_i = " << std::hex << top->data_i << std::endl;
            std::cout << "addr_a = " << top->addr_a << ", data_a = " << top->data_a << std::endl;
            std::cout << "addr_b = " << top->addr_b << ", data_b = " << top->data_b << std::dec << std::endl;
            std::cout << std::endl;
        }

        // Check values
        CHECK(25, top->data_a, 0x00000042);

        CHECK(36, top->data_a, 0x4200DEAD);

        CHECK(39, top->data_a, 0);
        CHECK(40, top->data_a, 0xAFFE1234);

        CHECK(52, top->data_b, 0xAFFE1234);

        CHECK(70, top->data_a, 0x12344200);

        CHECK(80, top->data_a, 0x12344200);

        CHECK(90, top->data_a, 0);

        SIM_ADV_TIME // Advance simulation time

    SIM_END

    TEST_PASSFAIL;

    delete top;
    exit(0);
}
