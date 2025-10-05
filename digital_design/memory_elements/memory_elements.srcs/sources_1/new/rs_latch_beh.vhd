----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 10/05/2025 12:11:55 PM
-- Design Name: 
-- Module Name: rs_latch_beh - Behavioral
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

entity rs_latch_beh is
    Port ( S : in STD_LOGIC;
           R : in STD_LOGIC;
           Q : out STD_LOGIC;
           nQ : out STD_LOGIC);
end rs_latch_beh;

architecture Behavioral of rs_latch_beh is
    signal state : STD_LOGIC := '0';
begin
    process(R, S)
    begin
        if R = '1' and S = '0' then
            state <= '0';
        elsif R = '0' and S = '1' then
            state <= '1';
        elsif R = '1' and S = '1' then
            state <= 'X';
        end if;
    end process;
    Q <= state;
    nQ <= not state;
end Behavioral;
