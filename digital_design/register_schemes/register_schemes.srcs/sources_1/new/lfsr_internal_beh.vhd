----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 08.10.2025 22:03:34
-- Design Name: 
-- Module Name: lfsr_internal_beh - Behavioral
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

entity lfsr_internal_beh is
    Port ( DOut : out STD_LOGIC;
           CLK : in STD_LOGIC;
           Reset : in STD_LOGIC;
           Load: in STD_LOGIC;
           Seed : in STD_LOGIC_VECTOR(0 to 2));
end lfsr_internal_beh;

architecture Behavioral of lfsr_internal_beh is

--f(x)= x^3 + x + 1
constant polynomial: std_logic_vector (0 to 2):="101";
signal trigger_data: std_logic_vector (0 to 2):="001";

begin
upd_process: process (CLK)
variable buff: std_logic_vector (trigger_data'range);
variable feedback:std_logic;
variable outbit: std_logic;
begin

    if (rising_edge(CLK)) then
        if Reset='1' then
            trigger_data<=(others=>'0');
        elsif Load='1' then
            trigger_data<=Seed;
        else
            buff:=trigger_data;
            outbit:=trigger_data(trigger_data'high);
            if outbit='0' then
               buff:= outbit & buff (buff'low to buff'high-1);
            else
                for i in buff'low to buff'high loop
                    if polynomial(i)='0' then
                        buff(i):=not buff(i);
                    end if;
                end loop;
                buff:= outbit & buff (buff'low to buff'high-1);
            end if;
            trigger_data<=buff;            
            DOut<=outbit;
        end if;      
    end if;
end process;

end Behavioral;
