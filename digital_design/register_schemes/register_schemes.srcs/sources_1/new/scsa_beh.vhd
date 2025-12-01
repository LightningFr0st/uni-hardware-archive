----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 09.10.2025 11:26:13
-- Design Name: 
-- Module Name: scsa_beh - Behavioral
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

entity scsa_beh is
    Port ( CLK : in STD_LOGIC;
           Reset : in STD_LOGIC;
           En : in STD_LOGIC;
           DIn : in STD_LOGIC;
           Load : in STD_LOGIC;
           Seed : in STD_LOGIC_VECTOR(0 to 2);
           Signature : out STD_LOGIC_VECTOR (0 to 2));
end scsa_beh;

architecture Behavioral of scsa_beh is

constant polynomial: std_logic_vector (0 to 2):="110";
signal trigger_data: std_logic_vector (0 to 2):=(others=>'0');

begin
upd_process: process (CLK)
variable buff: std_logic_vector (trigger_data'range);
variable feedback,acc: std_logic;
begin

    if (rising_edge(CLK)) then
        if Reset='1' then
            trigger_data<=(others=>'0');
        elsif Load='1' then
            trigger_data<=Seed;
        else
            buff:=trigger_data;
            acc:='0';
            for i in trigger_data'low to trigger_data'high loop
                if (polynomial(i)='1') then
                    acc:=acc xor trigger_data(i);
                end if;
            end loop;
            
            feedback:=DIn xor acc;
           
            for i in trigger_data'low+1 to trigger_data'high loop
                buff(i):=trigger_data(i-1);
            end loop;
            buff(buff'low):= feedback;
            trigger_data<=buff;
        end if;      
    end if;
end process;

Signature<=trigger_data;

end Behavioral;

