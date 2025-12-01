----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 08.10.2025 21:26:47
-- Design Name: 
-- Module Name: lfsr_external_beh - Behavioral
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

entity lfsr_external_beh is
    Port ( DOut : out STD_LOGIC;
           Q : out STD_LOGIC_VECTOR(2 downto 0);
           CLK : in STD_LOGIC;
           Reset : in STD_LOGIC;
           Load: in STD_LOGIC;
           Seed : in STD_LOGIC_VECTOR(2 downto 0));
end lfsr_external_beh;

architecture Behavioral of lfsr_external_beh is

--f(x)=x^3 + x + 1
constant polynomial: std_logic_vector (2 downto 0):="101";
signal trigger_data: std_logic_vector (2 downto 0):="001";

begin
upd_process: process (CLK)
variable buff: std_logic_vector (trigger_data'range);
variable feedback: std_logic;
variable outbit:std_logic;
begin

    if (rising_edge(CLK)) then
        if Reset='1' then
            trigger_data<=(others=>'0');
        elsif Load='1' then
            trigger_data<=Seed;
        else
            outbit:=trigger_data(trigger_data'low);
            
            buff:=trigger_data;
            feedback:='0';
            for i in trigger_data'high downto trigger_data'low loop
                if (polynomial(i)='1') then
                    feedback:=feedback xor trigger_data(i);
                end if;
            end loop;
            buff:=feedback & buff(buff'high downto buff'low+1);
            
            trigger_data<=buff;
            DOut<=outbit;

        end if;      
    end if;
end process;

   Q <= trigger_data;

end Behavioral;
