----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 08.10.2025 19:20:55
-- Design Name: 
-- Module Name: shift_register_beh - Behavioral
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

entity shift_register_beh is
    Port ( CLK : in STD_LOGIC;
           Reset : in STD_LOGIC;
           SE : in STD_LOGIC;
           DIn : in STD_LOGIC;
           DOut0 : out STD_LOGIC;
           DOut1 : out STD_LOGIC);
end shift_register_beh;

architecture Behavioral of shift_register_beh is

signal trigger_data: std_logic_vector(0 to 1):=(others=>'0');
begin
upd_process: process (CLK)
variable buff:std_logic_vector(trigger_data'range);
begin

    if(rising_edge(CLK)) then
        if Reset='1' then
            trigger_data<=(others=>'0');
        elsif SE='1' then
            buff:=trigger_data;
            for i in buff'low+1 to buff'high loop
                buff(i):=trigger_data(i-1);
            end loop;
            buff(0):=DIn;
            trigger_data<=buff;
        end if;
    end if;

end process;

DOut0<=trigger_data(0);
DOut1<=trigger_data(1);

end Behavioral;
