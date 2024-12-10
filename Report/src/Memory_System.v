module Memory_System(
    input UDS, LDS,
    input [11:0] Address,
    output [7:0] Data
);
    wire ROM00_SEL, ROM01_SEL, ROM10_SEL, ROM11_SEL;
    wire [7:0] ROM00_Data, ROM01_Data, ROM10_Data, ROM11_Data;

    wire [5:0] rom_address = Address[6:1];

    // ROM Selection Logic (Active Low)
    assign ROM00_SEL = ~((Address >= 12'h400) && (Address < 12'h440) && ~UDS);
    assign ROM01_SEL = ~((Address >= 12'h440) && (Address < 12'h480) && ~LDS);
    assign ROM10_SEL = ~((Address >= 12'h480) && (Address < 12'h4C0) && ~UDS);
    assign ROM11_SEL = ~((Address >= 12'h4C0) && (Address < 12'h500) && ~LDS);

    // Instantiate ROM Modules
    ROM0_1 ROM00 (.mem_address(rom_address), .CS(ROM00_SEL), .data_1(ROM00_Data));
    ROM0_2 ROM01 (.mem_address(rom_address), .CS(ROM01_SEL), .data_1(ROM01_Data));
    ROM1_1 ROM10 (.mem_address(rom_address), .CS(ROM10_SEL), .data_1(ROM10_Data));
    ROM1_2 ROM11 (.mem_address(rom_address), .CS(ROM11_SEL), .data_1(ROM11_Data));

    // Data Output Multiplexing
    assign Data = (ROM00_SEL == 0 ? ROM00_Data :
                  ROM01_SEL == 0 ? ROM01_Data :
                  ROM10_SEL == 0 ? ROM10_Data :
                  ROM11_SEL == 0 ? ROM11_Data : 8'bz);
endmodule
