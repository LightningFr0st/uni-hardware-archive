----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 09/21/2025 08:48:16 PM
-- Design Name: 
-- Module Name: mux4 - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity mux4 is
    Port ( in0_0 : in STD_LOGIC;
           in0_1 : in STD_LOGIC;
           in1_0 : in STD_LOGIC;
           in1_1 : in STD_LOGIC;
           sel : in STD_LOGIC;
           out0 : out STD_LOGIC;
           out1 : out STD_LOGIC);
end mux4;

architecture Behavioral of mux4 is

begin
    process(in0_0, in0_1, in1_0, in1_1, sel)
        begin
            if sel = '0' then
                out0 <= in0_0;
                out1 <= in0_1;
            else
                out0 <= in1_0;
                out1 <= in1_1;
            end if;
    end process;
end Behavioral;
