----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 09/22/2025 10:07:10 PM
-- Design Name: 
-- Module Name: demux_1x4_tb - Behavioral
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
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity demux_1x4_tb is
--  Port ( );
end demux_1x4_tb;

architecture Behavioral of demux_1x4_tb is

component demux_1x4_beh 
    Port ( I : in STD_LOGIC;
       S : in STD_LOGIC_VECTOR(1 downto 0);
       D : out STD_LOGIC_VECTOR(3 downto 0));
end component;

component demux_1x4_struct 
    Port ( I : in STD_LOGIC;
       S : in STD_LOGIC_VECTOR(1 downto 0);
       D : out STD_LOGIC_VECTOR(3 downto 0));
end component;

signal test_vector : std_logic_vector (2 downto 0);

signal i_int : std_logic;
signal s_int : std_logic_vector(1 downto 0);

signal d_struct : std_logic_vector(3 downto 0);
signal d_beh : std_logic_vector(3 downto 0);

signal error : std_logic;

constant period : time := 25 ns;

begin

    demux_beh: demux_1x4_beh port map(
        i => i_int,
        s => s_int,
        d => d_beh
     );
     
     demux_struct: demux_1x4_struct port map(
        i => i_int,
        s => s_int,
        d => d_struct
     );
     
     i_int <= test_vector(0);
     s_int <= test_vector(2 downto 1);
     
     error <= '1' when (d_struct /= d_beh) else '0';
     
     tb_proc : process
             variable error_count : integer := 0;
          begin
             
             report "Starting testbench...";
             
             for i in 0 to 8 loop
                 test_vector <= std_logic_vector(to_unsigned(i, test_vector'length));
                 wait for period;
                 
                 if error = '1' then
                     error_count := error_count + 1;
                     report "ERROR at test vector: " & integer'image(i);
                 end if;
             end loop;
             
             if error_count = 0 then
                 report "ALL TESTS PASSED! Both implementations are equivalent." severity note;
             else
                 report "TEST FAILED" severity note;
             end if;
             
             report "End of simulation" severity failure;
          
          end process;

end Behavioral;
